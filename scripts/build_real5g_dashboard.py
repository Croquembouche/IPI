#!/usr/bin/env python3
"""Build a static dashboard for Edge4AV real private-5G experiment results."""

import csv
import json
import math
import re
from collections import defaultdict
from pathlib import Path


RUN_ORDER = [
    "p5g-tcp-service-payload-0",
    "p5g-tcp-service-payload-256",
    "p5g-tcp-service-payload-1024",
    "p5g-tcp-service-payload-4096",
    "p5g-tcp-spat-baseline",
    "p5g-mqtt-service-baseline",
    "p5g-mqtt-spat-baseline",
    "loopback-tcp-service-baseline",
    "loopback-tcp-spat-baseline",
    "loopback-mqtt-service-baseline",
    "loopback-mqtt-spat-baseline",
]

DISPLAY_NAMES = {
    "p5g-tcp-service-payload-0": "5G TCP Service 0B",
    "p5g-tcp-service-payload-256": "5G TCP Service 256B",
    "p5g-tcp-service-payload-1024": "5G TCP Service 1024B",
    "p5g-tcp-service-payload-4096": "5G TCP Service 4096B",
    "p5g-tcp-spat-baseline": "5G TCP SPaT",
    "p5g-mqtt-service-baseline": "5G MQTT Service",
    "p5g-mqtt-spat-baseline": "5G MQTT SPaT",
    "loopback-tcp-service-baseline": "Loopback TCP Service",
    "loopback-tcp-spat-baseline": "Loopback TCP SPaT",
    "loopback-mqtt-service-baseline": "Loopback MQTT Service",
    "loopback-mqtt-spat-baseline": "Loopback MQTT SPaT",
}


def parse_bool(value):
    return str(value).strip().lower() in {"true", "1", "yes"}


def parse_float(value):
    if value is None or value == "":
        return None
    try:
        return float(value)
    except ValueError:
        return None


def percentile(values, pct):
    values = sorted(v for v in values if v is not None and v >= 0)
    if not values:
        return None
    index = max(0, min(len(values) - 1, math.ceil((pct / 100.0) * len(values)) - 1))
    return values[index]


def base_condition(condition_id):
    return re.sub(r"-trial-\d+$", "", condition_id)


def trial_number(condition_id):
    match = re.search(r"-trial-(\d+)$", condition_id)
    return int(match.group(1)) if match else None


def summarize(rows):
    rtt = [parse_float(row.get("rtt_ms")) for row in rows]
    uplink = [parse_float(row.get("uplink_ms")) for row in rows]
    downlink = [parse_float(row.get("downlink_ms")) for row in rows]
    accepted = sum(parse_bool(row.get("accepted")) for row in rows)
    attempts = len(rows)
    return {
        "attempts": attempts,
        "accepted": accepted,
        "successRate": 100.0 * accepted / attempts if attempts else 0.0,
        "rtt": {
            "p50": percentile(rtt, 50),
            "p95": percentile(rtt, 95),
            "p99": percentile(rtt, 99),
        },
        "uplink": {
            "p50": percentile(uplink, 50),
            "p95": percentile(uplink, 95),
            "p99": percentile(uplink, 99),
        },
        "downlink": {
            "p50": percentile(downlink, 50),
            "p95": percentile(downlink, 95),
            "p99": percentile(downlink, 99),
        },
    }


def build_data(run_dir):
    sender_files = sorted(run_dir.glob("*_sender.csv"))
    groups = defaultdict(list)
    trials = []

    for path in sender_files:
        if path.name.startswith("smoke"):
            continue
        with path.open(newline="") as handle:
            rows = list(csv.DictReader(handle))
        if not rows:
            continue
        condition_id = rows[0].get("condition_id", "")
        if condition_id.startswith("smoke"):
            continue
        base = base_condition(condition_id)
        trial = trial_number(condition_id)
        groups[base].extend(rows)
        trial_summary = summarize(rows)
        trials.append(
            {
                "condition": base,
                "conditionId": condition_id,
                "name": DISPLAY_NAMES.get(base, base),
                "trial": trial,
                "file": path.name,
                **trial_summary,
            }
        )

    aggregates = []
    for condition in RUN_ORDER:
        rows = groups.get(condition, [])
        if not rows:
            continue
        condition_trials = sorted(
            {
                trial_number(row.get("condition_id", ""))
                for row in rows
                if trial_number(row.get("condition_id", "")) is not None
            }
        )
        aggregate = summarize(rows)
        aggregates.append(
            {
                "condition": condition,
                "name": DISPLAY_NAMES.get(condition, condition),
                "trials": len(condition_trials),
                **aggregate,
            }
        )

    gps_latest = {}
    gps_path = run_dir / "gps" / "gps_latest.csv"
    if gps_path.exists():
        with gps_path.open(newline="") as handle:
            rows = list(csv.DictReader(handle))
            if rows:
                gps_latest = rows[-1]

    payload = []
    for item in aggregates:
        match = re.match(r"p5g-tcp-service-payload-(\d+)$", item["condition"])
        if match:
            payload.append({"payloadBytes": int(match.group(1)), **item})
    payload.sort(key=lambda item: item["payloadBytes"])

    comparisons = {
        "service": [
            item
            for item in aggregates
            if item["condition"]
            in {
                "p5g-tcp-service-payload-256",
                "p5g-mqtt-service-baseline",
                "loopback-tcp-service-baseline",
                "loopback-mqtt-service-baseline",
            }
        ],
        "spat": [
            item
            for item in aggregates
            if item["condition"]
            in {
                "p5g-tcp-spat-baseline",
                "p5g-mqtt-spat-baseline",
                "loopback-tcp-spat-baseline",
                "loopback-mqtt-spat-baseline",
            }
        ],
    }

    return {
        "runId": "edge4av-real-1000-repeat",
        "resultDir": str(run_dir),
        "design": {
            "trialsPerCondition": 3,
            "messagesPerTrial": 1000,
            "intervalMs": 200,
            "scope": "stationary network/control-plane latency",
        },
        "aggregates": aggregates,
        "trials": sorted(trials, key=lambda item: (RUN_ORDER.index(item["condition"]) if item["condition"] in RUN_ORDER else 999, item["trial"] or 0)),
        "payloadSensitivity": payload,
        "comparisons": comparisons,
        "gpsLatest": gps_latest,
    }


HTML = r"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Edge4AV Real 5G Results</title>
  <style>
    :root {
      --bg: #f7f7f2;
      --panel: #ffffff;
      --ink: #1f2933;
      --muted: #687684;
      --line: #d9ded8;
      --green: #24735b;
      --blue: #2f6f9f;
      --amber: #b46b22;
      --red: #b94d3a;
      --shadow: 0 1px 2px rgba(30, 41, 51, 0.08);
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      background: var(--bg);
      color: var(--ink);
      font-family: Inter, ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
      line-height: 1.35;
    }
    header {
      background: #12372f;
      color: white;
      padding: 22px 28px 18px;
      border-bottom: 4px solid #e5b15a;
    }
    h1 {
      margin: 0;
      font-size: 28px;
      font-weight: 720;
      letter-spacing: 0;
    }
    header p {
      margin: 7px 0 0;
      color: #d9e5df;
      max-width: 980px;
    }
    main {
      max-width: 1480px;
      margin: 0 auto;
      padding: 22px 24px 36px;
    }
    .toolbar {
      display: flex;
      flex-wrap: wrap;
      gap: 10px 18px;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 18px;
    }
    .tabs {
      display: inline-flex;
      gap: 4px;
      padding: 4px;
      background: #e9ede8;
      border: 1px solid var(--line);
      border-radius: 8px;
    }
    .tab {
      border: 0;
      background: transparent;
      color: var(--ink);
      padding: 8px 12px;
      border-radius: 6px;
      font-weight: 650;
      cursor: pointer;
    }
    .tab.active {
      background: var(--panel);
      box-shadow: var(--shadow);
    }
    .meta {
      color: var(--muted);
      font-size: 14px;
    }
    .grid {
      display: grid;
      grid-template-columns: repeat(12, 1fr);
      gap: 14px;
    }
    .panel {
      background: var(--panel);
      border: 1px solid var(--line);
      border-radius: 8px;
      box-shadow: var(--shadow);
      padding: 16px;
      min-width: 0;
    }
    .span-3 { grid-column: span 3; }
    .span-4 { grid-column: span 4; }
    .span-5 { grid-column: span 5; }
    .span-6 { grid-column: span 6; }
    .span-7 { grid-column: span 7; }
    .span-8 { grid-column: span 8; }
    .span-12 { grid-column: span 12; }
    .metric-title {
      color: var(--muted);
      font-size: 13px;
      font-weight: 720;
      text-transform: uppercase;
    }
    .metric-value {
      font-size: 26px;
      font-weight: 760;
      margin-top: 6px;
      white-space: nowrap;
    }
    .metric-sub {
      margin-top: 3px;
      color: var(--muted);
      font-size: 13px;
    }
    h2 {
      margin: 0 0 12px;
      font-size: 18px;
      letter-spacing: 0;
    }
    .chart {
      width: 100%;
      min-height: 270px;
      overflow: hidden;
    }
    svg { width: 100%; height: auto; display: block; }
    .axis { stroke: #9aa5a0; stroke-width: 1; }
    .grid-line { stroke: #edf0ec; stroke-width: 1; }
    .label { fill: var(--muted); font-size: 11px; }
    .bar-p50 { fill: var(--green); }
    .bar-p95 { fill: var(--blue); }
    .bar-p99 { fill: var(--amber); }
    .line-p50 { fill: none; stroke: var(--green); stroke-width: 3; }
    .line-p95 { fill: none; stroke: var(--blue); stroke-width: 3; }
    .line-p99 { fill: none; stroke: var(--amber); stroke-width: 3; }
    .dot-p50 { fill: var(--green); }
    .dot-p95 { fill: var(--blue); }
    .dot-p99 { fill: var(--amber); }
    .legend {
      display: flex;
      gap: 12px;
      flex-wrap: wrap;
      color: var(--muted);
      font-size: 13px;
      margin: -4px 0 10px;
    }
    .key {
      display: inline-flex;
      align-items: center;
      gap: 6px;
    }
    .swatch {
      width: 10px;
      height: 10px;
      border-radius: 2px;
      display: inline-block;
    }
    table {
      width: 100%;
      border-collapse: collapse;
      font-size: 13px;
    }
    th, td {
      border-bottom: 1px solid var(--line);
      padding: 8px 7px;
      text-align: right;
      vertical-align: top;
      white-space: nowrap;
    }
    th:first-child, td:first-child { text-align: left; white-space: normal; }
    th {
      color: var(--muted);
      font-weight: 720;
      background: #fbfcfa;
      position: sticky;
      top: 0;
      z-index: 1;
    }
    .table-wrap {
      max-height: 520px;
      overflow: auto;
      border: 1px solid var(--line);
      border-radius: 8px;
    }
    .note {
      color: var(--muted);
      font-size: 13px;
    }
    .gps {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 8px 14px;
      font-size: 14px;
    }
    .gps strong {
      display: block;
      color: var(--muted);
      font-size: 12px;
      text-transform: uppercase;
      margin-bottom: 3px;
    }
    .hidden { display: none; }
    @media (max-width: 1000px) {
      .span-3, .span-4, .span-5, .span-6, .span-7, .span-8 { grid-column: span 12; }
      main { padding: 16px; }
      .metric-value { font-size: 22px; }
      th, td { font-size: 12px; }
    }
  </style>
</head>
<body>
  <header>
    <h1>Edge4AV Real Private-5G Results</h1>
    <p>Stationary vehicle control-plane latency experiments with repeated 1000-message trials, transport comparison, payload sensitivity, host-loopback baseline, and GPS context.</p>
  </header>
  <main>
    <div class="toolbar">
      <div class="tabs">
        <button class="tab active" data-view="overview">Overview</button>
        <button class="tab" data-view="payload">Payload</button>
        <button class="tab" data-view="transport">Transport</button>
        <button class="tab" data-view="trials">Trials</button>
      </div>
      <div class="meta" id="runMeta"></div>
    </div>

    <section id="overview" class="view grid"></section>
    <section id="payload" class="view grid hidden"></section>
    <section id="transport" class="view grid hidden"></section>
    <section id="trials" class="view grid hidden"></section>
  </main>

  <script src="results-data.js"></script>
  <script>
    const data = window.EDGE4AV_RESULTS;
    const fmt = (value, digits = 2) => value === null || value === undefined || Number.isNaN(value) ? "n/a" : Number(value).toFixed(digits);
    const esc = (s) => String(s ?? "").replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));

    function metricCard(title, value, sub) {
      return `<div class="panel span-3"><div class="metric-title">${esc(title)}</div><div class="metric-value">${value}</div><div class="metric-sub">${esc(sub)}</div></div>`;
    }

    function chartFrame(title, body, span = 12, legend = true) {
      return `<div class="panel span-${span}"><h2>${esc(title)}</h2>${legend ? legendHtml() : ""}<div class="chart">${body}</div></div>`;
    }

    function legendHtml() {
      return `<div class="legend">
        <span class="key"><span class="swatch" style="background:var(--green)"></span>p50</span>
        <span class="key"><span class="swatch" style="background:var(--blue)"></span>p95</span>
        <span class="key"><span class="swatch" style="background:var(--amber)"></span>p99</span>
      </div>`;
    }

    function barChart(items, options = {}) {
      const width = 980, height = options.height || 360;
      const margin = {top: 14, right: 18, bottom: 112, left: 56};
      const innerW = width - margin.left - margin.right;
      const innerH = height - margin.top - margin.bottom;
      const maxValue = Math.max(1, ...items.flatMap(d => [d.rtt.p50 || 0, d.rtt.p95 || 0, d.rtt.p99 || 0]));
      const yMax = Math.ceil(maxValue * 1.12 / 10) * 10;
      const groupW = innerW / items.length;
      const barW = Math.max(5, Math.min(18, groupW / 5));
      const y = v => margin.top + innerH - (v / yMax) * innerH;
      const x = i => margin.left + i * groupW + groupW / 2;
      let svg = `<svg viewBox="0 0 ${width} ${height}" role="img">`;
      for (let tick = 0; tick <= 4; tick++) {
        const value = yMax * tick / 4;
        const yy = y(value);
        svg += `<line class="grid-line" x1="${margin.left}" y1="${yy}" x2="${width - margin.right}" y2="${yy}"></line>`;
        svg += `<text class="label" x="${margin.left - 8}" y="${yy + 4}" text-anchor="end">${fmt(value, 0)}</text>`;
      }
      svg += `<line class="axis" x1="${margin.left}" y1="${margin.top}" x2="${margin.left}" y2="${margin.top + innerH}"></line>`;
      svg += `<line class="axis" x1="${margin.left}" y1="${margin.top + innerH}" x2="${width - margin.right}" y2="${margin.top + innerH}"></line>`;
      items.forEach((d, i) => {
        const cx = x(i);
        [["p50", -barW * 1.2, "bar-p50"], ["p95", 0, "bar-p95"], ["p99", barW * 1.2, "bar-p99"]].forEach(([key, off, cls]) => {
          const value = d.rtt[key] || 0;
          const yy = y(value);
          svg += `<rect class="${cls}" x="${cx + off - barW / 2}" y="${yy}" width="${barW}" height="${margin.top + innerH - yy}" rx="2"></rect>`;
        });
        const label = options.shortLabels ? d.shortName || d.name : d.name;
        svg += `<text class="label" x="${cx}" y="${margin.top + innerH + 18}" text-anchor="end" transform="rotate(-36 ${cx} ${margin.top + innerH + 18})">${esc(label)}</text>`;
      });
      svg += `<text class="label" x="16" y="${margin.top + innerH / 2}" transform="rotate(-90 16 ${margin.top + innerH / 2})" text-anchor="middle">RTT ms</text>`;
      svg += `</svg>`;
      return svg;
    }

    function lineChart(items) {
      const width = 900, height = 340;
      const margin = {top: 18, right: 22, bottom: 52, left: 58};
      const innerW = width - margin.left - margin.right;
      const innerH = height - margin.top - margin.bottom;
      const maxValue = Math.max(1, ...items.flatMap(d => [d.rtt.p50 || 0, d.rtt.p95 || 0, d.rtt.p99 || 0]));
      const yMax = Math.ceil(maxValue * 1.12 / 10) * 10;
      const x = i => margin.left + (items.length === 1 ? innerW / 2 : i * innerW / (items.length - 1));
      const y = v => margin.top + innerH - (v / yMax) * innerH;
      let svg = `<svg viewBox="0 0 ${width} ${height}" role="img">`;
      for (let tick = 0; tick <= 4; tick++) {
        const value = yMax * tick / 4;
        const yy = y(value);
        svg += `<line class="grid-line" x1="${margin.left}" y1="${yy}" x2="${width - margin.right}" y2="${yy}"></line>`;
        svg += `<text class="label" x="${margin.left - 8}" y="${yy + 4}" text-anchor="end">${fmt(value, 0)}</text>`;
      }
      svg += `<line class="axis" x1="${margin.left}" y1="${margin.top + innerH}" x2="${width - margin.right}" y2="${margin.top + innerH}"></line>`;
      ["p50", "p95", "p99"].forEach(key => {
        const points = items.map((d, i) => `${x(i)},${y(d.rtt[key] || 0)}`).join(" ");
        svg += `<polyline class="line-${key}" points="${points}"></polyline>`;
        items.forEach((d, i) => svg += `<circle class="dot-${key}" cx="${x(i)}" cy="${y(d.rtt[key] || 0)}" r="4"></circle>`);
      });
      items.forEach((d, i) => {
        svg += `<text class="label" x="${x(i)}" y="${height - 22}" text-anchor="middle">${d.payloadBytes}</text>`;
      });
      svg += `<text class="label" x="${margin.left + innerW / 2}" y="${height - 4}" text-anchor="middle">payload bytes</text>`;
      svg += `<text class="label" x="16" y="${margin.top + innerH / 2}" transform="rotate(-90 16 ${margin.top + innerH / 2})" text-anchor="middle">RTT ms</text>`;
      svg += `</svg>`;
      return svg;
    }

    function table(headers, rows) {
      return `<div class="table-wrap"><table><thead><tr>${headers.map(h => `<th>${esc(h)}</th>`).join("")}</tr></thead><tbody>${rows.map(row => `<tr>${row.map(cell => `<td>${cell}</td>`).join("")}</tr>`).join("")}</tbody></table></div>`;
    }

    function renderOverview() {
      const totalAttempts = data.aggregates.reduce((sum, d) => sum + d.attempts, 0);
      const totalAccepted = data.aggregates.reduce((sum, d) => sum + d.accepted, 0);
      const realTcp = data.aggregates.find(d => d.condition === "p5g-tcp-service-payload-256");
      const realMqtt = data.aggregates.find(d => d.condition === "p5g-mqtt-service-baseline");
      const gps = data.gpsLatest || {};
      document.getElementById("overview").innerHTML =
        metricCard("Total Samples", totalAttempts.toLocaleString(), `${totalAccepted.toLocaleString()} accepted`) +
        metricCard("Success Rate", `${fmt(100 * totalAccepted / totalAttempts, 2)}%`, "all completed trials") +
        metricCard("5G TCP Service p95", `${fmt(realTcp.rtt.p95, 1)} ms`, "256 byte service payload") +
        metricCard("5G MQTT Service p95", `${fmt(realMqtt.rtt.p95, 1)} ms`, "256 byte service payload") +
        chartFrame("RTT Percentiles By Condition", barChart(data.aggregates, {height: 420, shortLabels: true}), 8) +
        `<div class="panel span-4"><h2>GPS Context</h2><div class="gps">
          <div><strong>Latitude</strong>${esc(gps.latitude_deg || "n/a")}</div>
          <div><strong>Longitude</strong>${esc(gps.longitude_deg || "n/a")}</div>
          <div><strong>Altitude</strong>${esc(gps.altitude_m || "n/a")} m</div>
          <div><strong>Speed</strong>${esc(gps.horizontal_speed_mps || "n/a")} m/s</div>
          <div><strong>Position Source</strong>${esc(gps.position_source || "n/a")}</div>
          <div><strong>Speed Source</strong>${esc(gps.speed_source || "n/a")}</div>
        </div><p class="note">GPS was recorded after the network trials because vehicle location was unchanged. Real 5G one-way timing is not synchronized; RTT is the reliable metric.</p></div>` +
        `<div class="panel span-12"><h2>Aggregate Table</h2>${aggregateTable(data.aggregates)}</div>`;
    }

    function aggregateTable(items) {
      return table(
        ["Condition", "Trials", "Attempts", "Accepted", "Success", "RTT p50", "RTT p95", "RTT p99"],
        items.map(d => [
          esc(d.name),
          d.trials,
          d.attempts.toLocaleString(),
          d.accepted.toLocaleString(),
          `${fmt(d.successRate, 2)}%`,
          `${fmt(d.rtt.p50, 3)} ms`,
          `${fmt(d.rtt.p95, 3)} ms`,
          `${fmt(d.rtt.p99, 3)} ms`,
        ])
      );
    }

    function renderPayload() {
      document.getElementById("payload").innerHTML =
        chartFrame("TCP Service Payload Sensitivity Over Real Private 5G", lineChart(data.payloadSensitivity), 7) +
        chartFrame("Payload Percentiles", barChart(data.payloadSensitivity, {height: 340}), 5) +
        `<div class="panel span-12"><h2>Payload Details</h2>${aggregateTable(data.payloadSensitivity)}</div>`;
    }

    function renderTransport() {
      const service = data.comparisons.service.map(shorten);
      const spat = data.comparisons.spat.map(shorten);
      document.getElementById("transport").innerHTML =
        chartFrame("Service Transport Comparison", barChart(service, {height: 340, shortLabels: true}), 6) +
        chartFrame("SPaT Transport Comparison", barChart(spat, {height: 340, shortLabels: true}), 6) +
        `<div class="panel span-12"><h2>Transport Table</h2>${aggregateTable([...service, ...spat])}</div>`;
    }

    function shorten(d) {
      const names = {
        "p5g-tcp-service-payload-256": "5G TCP",
        "p5g-mqtt-service-baseline": "5G MQTT",
        "loopback-tcp-service-baseline": "Loop TCP",
        "loopback-mqtt-service-baseline": "Loop MQTT",
        "p5g-tcp-spat-baseline": "5G TCP",
        "p5g-mqtt-spat-baseline": "5G MQTT",
        "loopback-tcp-spat-baseline": "Loop TCP",
        "loopback-mqtt-spat-baseline": "Loop MQTT",
      };
      return {...d, shortName: names[d.condition] || d.name};
    }

    function renderTrials() {
      document.getElementById("trials").innerHTML =
        `<div class="panel span-12"><h2>Per-Trial Completeness And RTT</h2>${table(
          ["Condition", "Trial", "Attempts", "Accepted", "Success", "RTT p50", "RTT p95", "RTT p99", "CSV"],
          data.trials.map(d => [
            esc(d.name),
            d.trial,
            d.attempts.toLocaleString(),
            d.accepted.toLocaleString(),
            `${fmt(d.successRate, 2)}%`,
            `${fmt(d.rtt.p50, 3)} ms`,
            `${fmt(d.rtt.p95, 3)} ms`,
            `${fmt(d.rtt.p99, 3)} ms`,
            esc(d.file),
          ])
        )}</div>`;
    }

    function switchView(view) {
      document.querySelectorAll(".tab").forEach(tab => tab.classList.toggle("active", tab.dataset.view === view));
      document.querySelectorAll(".view").forEach(section => section.classList.toggle("hidden", section.id !== view));
    }

    document.getElementById("runMeta").textContent = `${data.runId} · ${data.design.trialsPerCondition} trials × ${data.design.messagesPerTrial} messages · ${data.design.intervalMs} ms interval`;
    renderOverview();
    renderPayload();
    renderTransport();
    renderTrials();
    document.querySelectorAll(".tab").forEach(tab => tab.addEventListener("click", () => switchView(tab.dataset.view)));
  </script>
</body>
</html>
"""


def main():
    repo_root = Path(__file__).resolve().parents[1]
    run_dir = repo_root / "results" / "real_5g" / "20260425_173943"
    out_dir = run_dir / "dashboard"
    out_dir.mkdir(parents=True, exist_ok=True)

    data = build_data(run_dir)
    (out_dir / "results-data.js").write_text(
        "window.EDGE4AV_RESULTS = "
        + json.dumps(data, indent=2, sort_keys=True)
        + ";\n"
    )
    (out_dir / "index.html").write_text(HTML)
    print(out_dir)


if __name__ == "__main__":
    main()
