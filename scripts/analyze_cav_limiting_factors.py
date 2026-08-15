#!/usr/bin/env python3
"""Rebuild the Edge4AV limiting-factor analysis from stored result artifacts.

The script deliberately uses one row per experimental condition or valid
Airspan sample.  It never weights a condition by its attempt count, because a
50,000-attempt multiclient sample should not erase a 1,000-attempt payload
condition.  RTT percentiles use accepted replies only; all-attempt deadline
availability additionally requires acceptance.

PCA is descriptive: it identifies correlated outcome modes among conditions
with at least 20 successful replies.  The ordinal factor ranking is based on
matched contrasts plus an explicit evidence rubric.  Neither output is a
causal estimate of unmeasured radio-layer mechanisms.
"""

from __future__ import annotations

import csv
import json
import math
import re
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np


ROOT = Path(__file__).resolve().parents[1]
REAL5G = ROOT / "results" / "real_5g"
MOCAR = ROOT / "results" / "mocar_v2x"
BENCH = ROOT / "results" / "v2x_benchmarks"
OUT = ROOT / "paper" / "analysis" / "limiting_factors"

TRUE = {"true", "1", "yes", "accepted", "ok"}


def fnum(value, default=math.nan):
    try:
        if value is None or str(value).strip() == "":
            return default
        return float(value)
    except (TypeError, ValueError):
        return default


def inum(value, default=0):
    x = fnum(value, math.nan)
    return default if not math.isfinite(x) else int(x)


def is_true(value):
    return str(value).strip().lower() in TRUE


def quantiles(values):
    a = np.asarray([x for x in values if math.isfinite(x)], dtype=float)
    if not len(a):
        return (math.nan, math.nan, math.nan, math.nan)
    return tuple(float(x) for x in np.percentile(a, [50, 95, 99, 100]))


def longest_false_run(flags):
    best = cur = 0
    for flag in flags:
        if flag:
            cur = 0
        else:
            cur += 1
            best = max(best, cur)
    return best


def payload_from_text(text, fallback=0):
    m = re.search(r"payload[-_](\d+)", text)
    return int(m.group(1)) if m else fallback


def transport_from_text(text, fallback="unknown"):
    low = text.lower()
    for name in ("mqtt", "tcp", "udp"):
        if re.search(rf"(?:^|[-_/]){name}(?:[-_/]|$)", low):
            return name
    return fallback


def aggregate_sender_files(files, *, path_name, family, context,
                           formation="stream", clients=None,
                           load_mbps=0.0, moving=False, interrupted=False,
                           source_note=""):
    """Aggregate sender rows by condition_id across split/resumed files."""
    grouped = defaultdict(list)
    file_map = defaultdict(list)
    for file in files:
        if not file.exists() or file.stat().st_size == 0:
            continue
        with file.open(newline="", errors="replace") as handle:
            try:
                reader = csv.DictReader(handle)
                for row in reader:
                    if not row:
                        continue
                    cid = row.get("condition_id") or file.stem.replace("_sender", "")
                    grouped[cid].append(row)
                    file_map[cid].append(str(file.relative_to(ROOT)))
            except csv.Error:
                continue

    out = []
    for cid, rows in grouped.items():
        flags, rtts = [], []
        for row in rows:
            accepted = is_true(row.get("accepted", row.get("success", "")))
            flags.append(accepted)
            rtt = fnum(row.get("rtt_ms"))
            if accepted and math.isfinite(rtt):
                rtts.append(rtt)
        p50, p95, p99, rmax = quantiles(rtts)
        n = len(rows)
        accepted_n = sum(flags)
        sample = rows[0]
        requested = payload_from_text(cid, 0)
        client_count = clients
        if client_count is None:
            m = re.search(r"clients-(\d+)", cid)
            client_count = int(m.group(1)) if m else inum(
                sample.get("vehicle_outcome_value"), 1
            ) if sample.get("vehicle_outcome_name") == "clients" else 1
        transport = sample.get("transport") or transport_from_text(cid)
        all_rtts = [fnum(r.get("rtt_ms")) for r in rows]
        out.append({
            "path": path_name,
            "family": family,
            "run": rows[0].get("run_id") or files[0].parent.name,
            "condition": cid,
            "transport": transport,
            "formation": formation if transport == "udp" else "stream",
            "context": context,
            "payload_bytes": requested,
            "clients": client_count,
            "load_mbps": load_mbps,
            "moving": int(moving),
            "interrupted": int(interrupted),
            "attempts": n,
            "accepted": accepted_n,
            "success_rate": accepted_n / n if n else math.nan,
            "rtt_p50_ms": p50,
            "rtt_p95_ms": p95,
            "rtt_p99_ms": p99,
            "rtt_max_ms": rmax,
            "available_100ms": sum(
                flag and math.isfinite(rtt) and rtt <= 100
                for flag, rtt in zip(flags, all_rtts)
            ) / n if n else math.nan,
            "available_500ms": sum(
                flag and math.isfinite(rtt) and rtt <= 500
                for flag, rtt in zip(flags, all_rtts)
            ) / n if n else math.nan,
            "available_1000ms": sum(
                flag and math.isfinite(rtt) and rtt <= 1000
                for flag, rtt in zip(flags, all_rtts)
            ) / n if n else math.nan,
            "longest_failure_run": longest_false_run(flags),
            "source_files": ";".join(sorted(set(file_map[cid]))),
            "source_note": source_note,
        })
    return out


def collect_regular_5g():
    rows = []
    baselines = [
        "20260513_sunny_fintechparking_run_1",
        "20260514_sunny_after_rain_run_1",
        "20260515_sunny_run_1",
        "20260521_small_rain_run_1",
        "20260522_cloudy_run_1",
    ]
    contexts = {
        baselines[0]: "favorable_day_1",
        baselines[1]: "variable_day_2",
        baselines[2]: "adverse_variable_day",
        baselines[3]: "stable_day_4",
        baselines[4]: "stable_day_5",
    }
    for name in baselines:
        files = list((REAL5G / name).glob("*_sender.csv"))
        rows += aggregate_sender_files(
            files, path_name="5G Uu", family="payload_sweep",
            context=contexts[name],
            source_note="Stationary payload sweep; resumed files grouped by condition ID.",
        )

    detector_specs = [
        ("20260702_detector_output_to_ipi_run_1", "favorable", "stream"),
        ("20260702_detector_output_to_ipi_udp_fragmented_run_1", "favorable", "fragmented_udp"),
        ("20260706_detector_output_to_ipi_weak_signal_tcp_mqtt_run_1", "adverse_location", "stream"),
        ("20260706_detector_output_to_ipi_weak_signal_udp_fragmented_run_1", "adverse_location", "fragmented_udp"),
    ]
    for name, context, formation in detector_specs:
        files = list((REAL5G / name).glob("*_sender.csv"))
        rows += aggregate_sender_files(
            files, path_name="5G Uu", family="detector_replay",
            context=context, formation=formation,
            source_note="Detector-output-sized application replay.",
        )

    # Keep the completed raw-UDP diagnostic run as message-formation evidence.
    # The earlier raw-UDP run was explicitly aborted and is not included.
    raw_udp = "20260702_detector_output_to_ipi_udp_run_2"
    rows += aggregate_sender_files(
        list((REAL5G / raw_udp).glob("*_sender.csv")),
        path_name="5G Uu", family="message_formation_diagnostic",
        context="favorable", formation="raw_datagram",
        source_note="Completed diagnostic with intentionally unequal attempt counts; not a production reliability trial.",
    )

    load_specs = [
        "20260701_load_qos_run_1", "20260701_load_qos_run_2",
        "20260701_load_qos_run_3", "20260701_load_qos_run_4",
        "20260701_load_qos_weak_signal_run_1",
        "20260706_load_qos_weak_signal_run_2",
    ]
    for name in load_specs:
        root = REAL5G / name
        for file in root.glob("*_sender.csv"):
            stem = file.name.replace("_sender.csv", "")
            load_file = root / f"{stem}_load_client.csv"
            achieved = 0.0
            if load_file.exists():
                with load_file.open(newline="") as handle:
                    entries = list(csv.DictReader(handle))
                if entries:
                    achieved = fnum(entries[-1].get("throughput_mbps"), 0.0)
            # Some stream-count runs have load logs in condition directories;
            # use zero when no achieved-rate artifact can be matched.  The
            # requested stream count is not substituted for measured Mbps.
            context = "adverse_location" if "weak_signal" in name else "favorable"
            rows += aggregate_sender_files(
                [file], path_name="5G Uu", family="uplink_load",
                context=context, load_mbps=achieved,
                source_note="Actual load Mbps is used when a matching load-client record exists.",
            )

    for name, context in [
        ("20260702_multiclient_scalability_run_4", "favorable"),
        ("20260706_multiclient_scalability_weak_signal_run_1", "adverse_location"),
    ]:
        rows += aggregate_sender_files(
            list((REAL5G / name).glob("*_sender.csv")),
            path_name="5G Uu", family="logical_clients", context=context,
            source_note="Logical clients share one vehicle UE; they are not independent UEs.",
        )

    failure_root = REAL5G / "20260703_failure_fallback_run_1"
    rows += aggregate_sender_files(
        list(failure_root.glob("*_sender.csv")), path_name="5G Uu",
        family="restart", context="stationary", interrupted=True,
        source_note="One injected restart per condition; no fallback path was tested.",
    )
    return rows


def collect_airspan():
    rows = []
    for file in sorted(REAL5G.glob("20260806_airspan*/analysis/application_summary.csv")):
        run = file.parents[1].name
        with file.open(newline="") as handle:
            for r in csv.DictReader(handle):
                if not is_true(r.get("application_sample_valid")):
                    continue
                n = inum(r.get("attempts"))
                accepted = inum(r.get("accepted"))
                context = r.get("corrected_signal_class") or "unknown"
                rows.append({
                    "path": "5G Uu",
                    "family": "airspan_followup",
                    "run": run,
                    "condition": r.get("sample_id", ""),
                    "transport": r.get("transport", "unknown"),
                    "formation": "datagram" if r.get("transport") == "udp" else "stream",
                    "context": context,
                    "payload_bytes": inum(r.get("payload_bytes")),
                    "clients": inum(r.get("clients"), 1),
                    "load_mbps": fnum(r.get("load_report_mbps"), 0.0),
                    "moving": 0,
                    "interrupted": 0,
                    "attempts": n,
                    "accepted": accepted,
                    "success_rate": accepted / n if n else math.nan,
                    "rtt_p50_ms": fnum(r.get("rtt_p50_ms")),
                    "rtt_p95_ms": fnum(r.get("rtt_p95_ms")),
                    "rtt_p99_ms": fnum(r.get("rtt_p99_ms")),
                    "rtt_max_ms": fnum(r.get("rtt_max_ms")),
                    "available_100ms": fnum(r.get("deadline_hit_rate_100ms_pct")) / 100,
                    "available_500ms": fnum(r.get("deadline_hit_rate_500ms_pct")) / 100,
                    "available_1000ms": fnum(r.get("deadline_hit_rate_1000ms_pct")) / 100,
                    "longest_failure_run": math.nan,
                    "source_files": str(file.relative_to(ROOT)),
                    "source_note": "Valid follow-up sample under operator-reported 70/20/10; context label is not MG52 RF telemetry.",
                })
    return rows


def collect_pc5():
    rows = []
    # The final reference summary consolidates the July 3 stable payload runs.
    ref = MOCAR / "20260703_exp_01_payload_sweep_0_2kb_final" / "payload_summary.csv"
    with ref.open(newline="") as handle:
        for r in csv.DictReader(handle):
            n = inum(r.get("rows"))
            accepted = inum(r.get("success"))
            rows.append({
                "path": "PC5 direct", "family": "pc5_stationary_reference",
                "run": ref.parents[0].name,
                "condition": f"reference-payload-{r['payload_bytes']}",
                "transport": "pc5_custom_echo", "formation": "vendor_stack",
                "context": "stable_reference", "payload_bytes": inum(r["payload_bytes"]),
                "clients": 1, "load_mbps": 0.0, "moving": 0, "interrupted": 0,
                "attempts": n, "accepted": accepted,
                "success_rate": accepted / n if n else math.nan,
                "rtt_p50_ms": fnum(r.get("p50_rtt_ms")),
                "rtt_p95_ms": fnum(r.get("p95_rtt_ms")),
                "rtt_p99_ms": fnum(r.get("p99_rtt_ms")),
                "rtt_max_ms": math.nan,
                "available_100ms": math.nan, "available_500ms": math.nan,
                "available_1000ms": math.nan, "longest_failure_run": math.nan,
                "source_files": str(ref.relative_to(ROOT)),
                "source_note": "Derived consolidation of unique stable July 3 runs; source runs are not counted again.",
            })

    stationary = [
        ("20260704_exp_01_payload_sweep_0_2kb_133527", "P1_obstructed_NLOS_68m"),
        ("20260704_exp_01_payload_sweep_0_2kb_144149", "P2_113m"),
        ("20260704_exp_01_payload_sweep_0_2kb_151351", "P3_212m"),
        ("20260704_exp_01_payload_sweep_0_2kb_155120", "P4_377m"),
        ("20260704_exp_01_payload_sweep_0_2kb_164129", "P5_469m"),
    ]
    for name, context in stationary:
        file = MOCAR / name / "payload_summary.csv"
        if not file.exists():
            continue
        with file.open(newline="") as handle:
            for r in csv.DictReader(handle):
                # Some stopped sweeps report a 1,000-attempt analysis
                # denominator.  Preserve only raw observed attempts here.
                n = inum(r.get("observed_rows", r.get("rows")))
                if n <= 0:
                    # Skipped synthetic timeout rows are not observations.
                    continue
                accepted = inum(r.get("success_for_analysis", r.get("success")))
                rows.append({
                    "path": "PC5 direct", "family": "pc5_stationary",
                    "run": name, "condition": f"{context}-payload-{r['payload_bytes']}",
                    "transport": "pc5_custom_echo", "formation": "vendor_stack",
                    "context": context, "payload_bytes": inum(r["payload_bytes"]),
                    "clients": 1, "load_mbps": 0.0, "moving": 0, "interrupted": 0,
                    "attempts": n, "accepted": accepted,
                    "success_rate": accepted / n if n else math.nan,
                    "rtt_p50_ms": fnum(r.get("p50_rtt_ms")),
                    "rtt_p95_ms": fnum(r.get("p95_rtt_ms")),
                    "rtt_p99_ms": fnum(r.get("p99_rtt_ms")),
                    "rtt_max_ms": math.nan,
                    "available_100ms": math.nan, "available_500ms": math.nan,
                    "available_1000ms": math.nan, "longest_failure_run": math.nan,
                    "source_files": str(file.relative_to(ROOT)),
                    "source_note": "Observed rows only; operator-skipped remainder is excluded.",
                })

    for file in sorted(MOCAR.glob("20260704_exp_02_radio_distance_mobility_*/radio_mobility_summary.csv")):
        with file.open(newline="") as handle:
            for r in csv.DictReader(handle):
                n = inum(r.get("rows"))
                accepted = inum(r.get("success"))
                rows.append({
                    "path": "PC5 direct", "family": "pc5_mobility",
                    "run": file.parent.name, "condition": r.get("condition_id", ""),
                    "transport": "pc5_custom_echo", "formation": "vendor_stack",
                    "context": "moving_route", "payload_bytes": 256,
                    "clients": 1, "load_mbps": 0.0, "moving": 1, "interrupted": 0,
                    "attempts": n, "accepted": accepted,
                    "success_rate": accepted / n if n else math.nan,
                    "rtt_p50_ms": fnum(r.get("p50_rtt_ms")),
                    "rtt_p95_ms": fnum(r.get("p95_rtt_ms")),
                    "rtt_p99_ms": fnum(r.get("p99_rtt_ms")),
                    "rtt_max_ms": math.nan,
                    "available_100ms": math.nan, "available_500ms": math.nan,
                    "available_1000ms": math.nan, "longest_failure_run": math.nan,
                    "source_files": str(file.relative_to(ROOT)),
                    "source_note": "Route-level accepted echo; speed and position are confounded.",
                })
    return rows


def write_csv(path, rows, fields=None):
    rows = list(rows)
    if fields is None:
        fields = list(rows[0]) if rows else []
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields, extrasaction="ignore")
        writer.writeheader()
        writer.writerows(rows)


def pca_analysis(rows):
    features = [
        "log10_p50", "log10_p95", "log10_p99", "tail_log10_p95_p50",
        "failure_fraction",
    ]
    eligible = [r for r in rows if r["accepted"] >= 20 and all(
        math.isfinite(r[k]) for k in ("rtt_p50_ms", "rtt_p95_ms", "rtt_p99_ms")
    )]
    matrix = []
    for r in eligible:
        p50, p95, p99 = r["rtt_p50_ms"], r["rtt_p95_ms"], r["rtt_p99_ms"]
        matrix.append([
            math.log10(max(p50, 0.001)), math.log10(max(p95, 0.001)),
            math.log10(max(p99, 0.001)), math.log10(max(p95 / max(p50, 0.001), 0.001)),
            1 - r["success_rate"],
        ])
    x = np.asarray(matrix, dtype=float)
    means, stds = x.mean(axis=0), x.std(axis=0, ddof=1)
    stds[stds == 0] = 1
    z = (x - means) / stds
    cov = np.cov(z, rowvar=False)
    vals, vecs = np.linalg.eigh(cov)
    order = np.argsort(vals)[::-1]
    vals, vecs = vals[order], vecs[:, order]
    # Make positive PC1 consistently mean worse latency/misses.
    if vecs[:, 0].sum() < 0:
        vecs[:, 0] *= -1
    # Orient PC2 toward failure/outage and PC3 toward tail inflation so the
    # signs remain stable across rebuilds.  Component signs are otherwise
    # mathematically arbitrary.
    if vecs[-1, 1] < 0:
        vecs[:, 1] *= -1
    if vecs[3, 2] < 0:
        vecs[:, 2] *= -1
    scores = z @ vecs
    explained = vals / vals.sum()

    loadings = []
    for i, feature in enumerate(features):
        loadings.append({
            "outcome_feature": feature,
            "PC1_loading": vecs[i, 0], "PC2_loading": vecs[i, 1],
            "PC3_loading": vecs[i, 2],
            "PC1_explained_variance": explained[0],
            "PC2_explained_variance": explained[1],
            "PC3_explained_variance": explained[2],
        })
    score_rows = []
    for r, s in zip(eligible, scores):
        score_rows.append({
            "path": r["path"], "family": r["family"], "run": r["run"],
            "condition": r["condition"], "context": r["context"],
            "transport": r["transport"], "payload_bytes": r["payload_bytes"],
            "clients": r["clients"], "load_mbps": r["load_mbps"],
            "PC1_burden": s[0], "PC2": s[1], "PC3": s[2],
        })
    return loadings, score_rows, explained


def add_contrast(rows, factor, evidence, before, after, design, source):
    before_p95 = fnum(before.get("rtt_p95_ms")) if before else math.nan
    after_p95 = fnum(after.get("rtt_p95_ms")) if after else math.nan
    before_s = fnum(before.get("success_rate")) if before else math.nan
    after_s = fnum(after.get("success_rate")) if after else math.nan
    rows.append({
        "factor": factor, "evidence": evidence, "design": design,
        "before_p95_ms": before_p95, "after_p95_ms": after_p95,
        "p95_ratio": after_p95 / before_p95 if before_p95 > 0 and math.isfinite(after_p95) else math.nan,
        "before_success_pct": 100 * before_s if math.isfinite(before_s) else math.nan,
        "after_success_pct": 100 * after_s if math.isfinite(after_s) else math.nan,
        "success_change_pp": 100 * (after_s - before_s) if math.isfinite(before_s) and math.isfinite(after_s) else math.nan,
        "source": source,
    })


def select(rows, **kwargs):
    return [r for r in rows if all(r.get(k) == v for k, v in kwargs.items())]


def median_condition(items):
    if not items:
        return None
    out = dict(items[0])
    for field in ("rtt_p50_ms", "rtt_p95_ms", "rtt_p99_ms", "success_rate"):
        vals = [fnum(x.get(field)) for x in items]
        vals = [x for x in vals if math.isfinite(x)]
        out[field] = float(np.median(vals)) if vals else math.nan
    return out


def build_contrasts(rows):
    contrasts = []
    # Repeated baseline payload sweeps: compare smallest and largest completed
    # payload within each run and transport.
    for run in sorted({r["run"] for r in rows if r["family"] == "payload_sweep"}):
        for transport in ("tcp", "mqtt"):
            items = select(rows, family="payload_sweep", run=run, transport=transport)
            items = [r for r in items if r["attempts"] >= 900 and math.isfinite(r["rtt_p95_ms"])]
            if len(items) < 2:
                continue
            low, high = min(items, key=lambda x: x["payload_bytes"]), max(items, key=lambda x: x["payload_bytes"])
            add_contrast(
                contrasts, "payload and representation scale",
                f"{run}: {transport} {low['payload_bytes']}B to {high['payload_bytes']}B",
                low, high, "within run and transport", f"{low['source_files']};{high['source_files']}",
            )

    # Canonical logical-client contrast.
    for context, run in [
        ("favorable", "edge4av-real-20260702-multiclient-scalability-run-4"),
        ("adverse_location", "edge4av-real-20260706-multiclient-scalability-weak-signal-run-1"),
    ]:
        for transport in ("tcp", "mqtt", "udp"):
            one = median_condition([r for r in rows if r["run"] == run and r["transport"] == transport and r["clients"] == 1])
            hundred = median_condition([r for r in rows if r["run"] == run and r["transport"] == transport and r["clients"] == 100])
            if one and hundred:
                add_contrast(
                    contrasts, "concurrent demand under no verified resource isolation",
                    f"{context}: {transport}, 1 to 100 logical clients",
                    one, hundred, "within run; clients share one UE",
                    f"{one['source_files']};{hundred['source_files']}",
                )

    # Airspan C1 versus C3 (load) and C1 versus C4 (logical clients).
    air_runs = sorted({r["run"] for r in rows if r["family"] == "airspan_followup"})
    for run in air_runs:
        for transport in ("tcp", "mqtt", "udp"):
            c1 = median_condition([r for r in rows if r["run"] == run and r["transport"] == transport and r["condition"].startswith("C1-")])
            c3 = median_condition([r for r in rows if r["run"] == run and r["transport"] == transport and r["condition"].startswith("C3-")])
            c4 = median_condition([r for r in rows if r["run"] == run and r["transport"] == transport and r["condition"].startswith("C4-")])
            if c1 and c3:
                add_contrast(contrasts, "concurrent demand under no verified resource isolation", f"{run}: {transport}, C1 idle to C3 uplink load", c1, c3, "same-day fixed-location repetitions", f"{c1['source_files']};{c3['source_files']}")
            if c1 and c4:
                add_contrast(contrasts, "concurrent demand under no verified resource isolation", f"{run}: {transport}, C1 one client to C4 100 logical clients", c1, c4, "same-day fixed-location repetitions; one UE", f"{c1['source_files']};{c4['source_files']}")

    # Original load run 1/2: idle versus the actually achieved background
    # uplink rate.  Requested rate is not used as the independent variable.
    for run in sorted({r["run"] for r in rows if r["family"] == "uplink_load"}):
        for transport in ("tcp", "mqtt"):
            items = [r for r in rows if r["run"] == run and r["transport"] == transport]
            idle = next((r for r in items if "idle" in r["condition"]), None)
            active = max((r for r in items if r["load_mbps"] > 0), key=lambda x: x["load_mbps"], default=None)
            if idle and active:
                add_contrast(
                    contrasts, "concurrent demand under no verified resource isolation",
                    f"{run}: {transport}, idle to {active['load_mbps']:.3f} Mbps achieved uplink load",
                    idle, active, "within run and transport; measured load rate",
                    f"{idle['source_files']};{active['source_files']}",
                )

    # Detector replay across contexts and sizes.
    for transport in ("tcp", "mqtt", "udp"):
        for context in ("favorable", "adverse_location"):
            items = [r for r in rows if r["family"] == "detector_replay" and r["transport"] == transport and r["context"] == context]
            lo = next((r for r in items if r["payload_bytes"] == 4096), None)
            hi = next((r for r in items if r["payload_bytes"] == 60000), None)
            if lo and hi:
                add_contrast(contrasts, "payload and representation scale", f"detector replay {context}: {transport}, 4KiB to 60KiB", lo, hi, "within run/transport", f"{lo['source_files']};{hi['source_files']}")

    # PC5 same-point payload sensitivity.
    for context in ("stable_reference", "P2_113m", "P3_212m", "P4_377m"):
        items = [r for r in rows if r["path"] == "PC5 direct" and r["context"] == context]
        if len(items) >= 2:
            lo, hi = min(items, key=lambda x: x["payload_bytes"]), max(items, key=lambda x: x["payload_bytes"])
            add_contrast(contrasts, "payload and representation scale", f"PC5 {context}: {lo['payload_bytes']}B to {hi['payload_bytes']}B", lo, hi, "same point; ascending-order sweep", f"{lo['source_files']};{hi['source_files']}")

    # PC5 spatial effect at fixed payloads; use location rather than distance as
    # the factor because P1 was worse than P2 and there are no RF counters.
    for payload in (256, 512, 1024, 2048):
        p2 = next((r for r in rows if r["context"] == "P2_113m" and r["payload_bytes"] == payload), None)
        farther = next((r for r in rows if r["context"] == "P4_377m" and r["payload_bytes"] == payload), None)
        if p2 and farther:
            add_contrast(contrasts, "operating path and spatial availability", f"PC5 P2 to P4 at {payload}B", p2, farther, "different field points; no RF counters", f"{p2['source_files']};{farther['source_files']}")

    # Raw datagrams versus the application-fragmented path at identical
    # requested sizes.  RTT ratios are intentionally absent when raw UDP had no
    # accepted replies; the success-rate change carries the result.
    for payload in (1024, 4096, 19648):
        raw = next((r for r in rows if r["family"] == "message_formation_diagnostic" and r["payload_bytes"] == payload), None)
        fragmented = next((r for r in rows if r["family"] == "detector_replay" and r["formation"] == "fragmented_udp" and r["context"] == "favorable" and r["payload_bytes"] == payload), None)
        if raw and fragmented:
            add_contrast(
                contrasts, "message formation and transport semantics",
                f"raw versus fragmented UDP at {payload}B", raw, fragmented,
                "different diagnostic runs; same requested payload", f"{raw['source_files']};{fragmented['source_files']}",
            )

    # Restart rows are compared with their accepted-reply tail only to preserve
    # outage loss.  The measured success gap is recorded separately in report.
    for r in [x for x in rows if x["family"] == "restart"]:
        baseline = dict(r)
        baseline["success_rate"] = 1.0
        baseline["rtt_p95_ms"] = r["rtt_p95_ms"]
        add_contrast(contrasts, "service interruption and continuity", r["condition"], baseline, r, "one injected restart", r["source_files"])
    return contrasts


def factor_ranking():
    weights = {
        "observed_severity": 0.30, "repeatability": 0.20,
        "evidence_breadth": 0.20, "identification_strength": 0.15,
        "complex_cav_relevance": 0.15,
    }
    factors = [
        {
            "factor": "Payload and representation scale",
            "observed_severity": 5.0, "repeatability": 5.0, "evidence_breadth": 5.0,
            "identification_strength": 4.0, "complex_cav_relevance": 5.0,
            "evidence_grade": "A-",
            "reason": "Repeated 5G sweeps, detector replays, PC5 sweeps, and workload-size inventory all expose the same pressure.",
        },
        {
            "factor": "Concurrent demand under no verified resource isolation",
            "observed_severity": 5.0, "repeatability": 5.0, "evidence_breadth": 4.0,
            "identification_strength": 4.5, "complex_cav_relevance": 5.0,
            "evidence_grade": "A-",
            "reason": "Within-run load and client-count contrasts repeatedly enlarge tails; logical clients share one UE and no nondefault bearer was verified.",
        },
        {
            "factor": "Operating path and spatial availability",
            "observed_severity": 5.0, "repeatability": 4.5, "evidence_breadth": 5.0,
            "identification_strength": 2.5, "complex_cav_relevance": 5.0,
            "evidence_grade": "B",
            "reason": "Both paths vary strongly by field context, but PC5 RF counters and matched 5G UE radio telemetry are unavailable.",
        },
        {
            "factor": "Service interruption and state continuity",
            "observed_severity": 5.0, "repeatability": 5.0, "evidence_breadth": 2.0,
            "identification_strength": 4.5, "complex_cav_relevance": 5.0,
            "evidence_grade": "B+",
            "reason": "All four injected restarts created 11.7-15.1 s success gaps, but there was one injection per condition and no fallback.",
        },
        {
            "factor": "Message formation and transport semantics",
            "observed_severity": 5.0, "repeatability": 4.0, "evidence_breadth": 3.0,
            "identification_strength": 3.5, "complex_cav_relevance": 4.0,
            "evidence_grade": "B",
            "reason": "Raw UDP failed beyond its practical datagram boundary while fragmented UDP restored smaller transfers but not 60 KiB reliability.",
        },
        {
            "factor": "Directional capacity and frame allocation",
            "observed_severity": 4.0, "repeatability": 3.0, "evidence_breadth": 3.0,
            "identification_strength": 2.5, "complex_cav_relevance": 5.0,
            "evidence_grade": "C+",
            "reason": "Host tests show uplink/downlink asymmetry, but the TDD profiles lack a matched same-cell causal comparison and cell counters.",
        },
        {
            "factor": "Local perception compute time (conditional)",
            "observed_severity": 5.0, "repeatability": 5.0, "evidence_breadth": 1.0,
            "identification_strength": 3.0, "complex_cav_relevance": 2.0,
            "evidence_grade": "B-",
            "reason": "One full V2X-Radar detector configuration took seconds per sample; it is not an end-to-end or universal compute result.",
        },
    ]
    for row in factors:
        row["weighted_score_100"] = 20 * sum(row[k] * w for k, w in weights.items())
    factors.sort(key=lambda r: r["weighted_score_100"], reverse=True)
    for rank, row in enumerate(factors, 1):
        row["rank"] = rank

    # Weight sensitivity: perturb the stated weights by independent factors in
    # [0.5, 1.5], then normalize.  This tests whether the ordinal result is an
    # artifact of one exact weighting choice.
    rng = np.random.default_rng(20260813)
    keys = list(weights)
    base = np.asarray([weights[k] for k in keys])
    rank_counts = {r["factor"]: np.zeros(len(factors), dtype=int) for r in factors}
    for _ in range(20000):
        w = base * rng.uniform(0.5, 1.5, size=len(base))
        w /= w.sum()
        scored = sorted(
            factors,
            key=lambda r: sum(r[k] * w[i] for i, k in enumerate(keys)),
            reverse=True,
        )
        for i, r in enumerate(scored):
            rank_counts[r["factor"]][i] += 1
    for row in factors:
        counts = rank_counts[row["factor"]]
        row["rank1_probability"] = counts[0] / counts.sum()
        row["top3_probability"] = counts[:3].sum() / counts.sum()
        row["sensitivity_min_rank"] = int(np.flatnonzero(counts)[0] + 1)
        row["sensitivity_max_rank"] = int(np.flatnonzero(counts)[-1] + 1)
    return factors, weights


def benchmark_summary():
    manifest = json.loads((BENCH / "latest" / "v2x_ipi_payload_manifest.json").read_text())
    records = manifest["records"]
    sizes = np.asarray([int(r["raw_bytes"]) for r in records], dtype=np.int64)
    chunks = np.asarray([int(r["ipi_chunk_count"]) for r in records], dtype=np.int64)
    detector_file = BENCH / "v2x-radar-detector-benchmark-20260629T182624Z" / "per_sample.csv"
    times = []
    with detector_file.open(newline="") as handle:
        for r in csv.DictReader(handle):
            if r.get("status") == "ok":
                times.append(fnum(r.get("elapsed_ms")))
    t = np.asarray(times, dtype=float)
    return {
        "workload_manifest": {
            "files": int(len(sizes)), "min_bytes": int(sizes.min()),
            "median_bytes": float(np.median(sizes)), "p95_bytes": float(np.percentile(sizes, 95)),
            "p99_bytes": float(np.percentile(sizes, 99)), "max_bytes": int(sizes.max()),
            "mean_bytes": float(sizes.mean()), "over_60000_count": int((sizes > 60000).sum()),
            "over_60000_fraction": float((sizes > 60000).mean()),
            "chunk_median": float(np.median(chunks)), "chunk_p95": float(np.percentile(chunks, 95)),
            "chunk_max": int(chunks.max()),
            "boundary": "Raw public-dataset file sizes and estimated IPI chunk counts; not transmitted application messages.",
        },
        "detector_compute": {
            "samples": int(len(t)), "min_ms": float(t.min()), "median_ms": float(np.median(t)),
            "p95_ms": float(np.percentile(t, 95)), "p99_ms": float(np.percentile(t, 99)),
            "max_ms": float(t.max()), "mean_ms": float(t.mean()),
            "boundary": "Offline local V2X-Radar detector on four RTX 2080 Ti GPUs; not end-to-end and not added to network RTT.",
        },
    }


def pc5_spatial_bins():
    """Aggregate joined mobility attempts by distance from the RSU marker."""
    # Repository testbed-map RSU marker.  This is the direct-radio RSU, not the
    # separate private-5G base-station marker used by the iPhone survey.
    base_lat, base_lon = 39.66711689714955, -75.75772185598035

    def distance_m(lat, lon):
        radius = 6_371_000.0
        p1, p2 = math.radians(base_lat), math.radians(lat)
        dp = math.radians(lat - base_lat)
        dl = math.radians(lon - base_lon)
        a = math.sin(dp / 2) ** 2 + math.cos(p1) * math.cos(p2) * math.sin(dl / 2) ** 2
        return 2 * radius * math.asin(math.sqrt(a))

    out = []
    for file in sorted(MOCAR.glob("20260704_exp_02_radio_distance_mobility_*/v2x_gnss_by_send_time.csv")):
        bins = defaultdict(list)
        with file.open(newline="") as handle:
            for r in csv.DictReader(handle):
                lat, lon = fnum(r.get("send_latitude_deg")), fnum(r.get("send_longitude_deg"))
                if not (math.isfinite(lat) and math.isfinite(lon)):
                    continue
                dist = distance_m(lat, lon)
                lower = int(dist // 100) * 100
                lower = min(lower, 500)
                bins[lower].append((is_true(r.get("success")), fnum(r.get("rtt_ms"))))
        for lower, samples in sorted(bins.items()):
            flags = [x[0] for x in samples]
            rtts = [x[1] for x in samples if x[0] and math.isfinite(x[1])]
            p50, p95, p99, _ = quantiles(rtts)
            out.append({
                "run": file.parent.name, "distance_bin_start_m": lower,
                "distance_bin_end_m": lower + 100, "attempts": len(samples),
                "accepted": sum(flags), "success_rate": sum(flags) / len(flags),
                "rtt_p50_ms_successful": p50, "rtt_p95_ms_successful": p95,
                "rtt_p99_ms_successful": p99,
                "longest_consecutive_failure_run": longest_false_run(flags),
                "source": str(file.relative_to(ROOT)),
            })
    return out


def make_figures(rankings, loadings, score_rows, contrasts, spatial_bins):
    plt.rcParams.update({"font.size": 10, "axes.titlesize": 12, "axes.labelsize": 10})

    fig, ax = plt.subplots(figsize=(9.4, 5.2))
    labels = [r["factor"] for r in rankings][::-1]
    scores = [r["weighted_score_100"] for r in rankings][::-1]
    colors = ["#4063D8" if r["rank"] <= 3 else "#7B8AA0" for r in rankings][::-1]
    bars = ax.barh(labels, scores, color=colors)
    ax.set_xlim(0, 100)
    ax.set_xlabel("Evidence-weighted score (0–100; ordinal ranking aid)")
    ax.set_title("Observed limiting factors for complex CAV applications")
    ax.grid(axis="x", alpha=0.25)
    for bar, score in zip(bars, scores):
        ax.text(score + 1, bar.get_y() + bar.get_height() / 2, f"{score:.1f}", va="center")
    fig.tight_layout()
    fig.savefig(OUT / "factor_ranking.png", dpi=220, bbox_inches="tight")
    plt.close(fig)

    fig, axes = plt.subplots(1, 2, figsize=(10.8, 4.8))
    paths = sorted({r["path"] for r in score_rows})
    colors_by_path = {"5G Uu": "#4063D8", "PC5 direct": "#D94F4F"}
    for path in paths:
        sub = [r for r in score_rows if r["path"] == path]
        axes[0].scatter([r["PC1_burden"] for r in sub], [r["PC2"] for r in sub],
                        s=18, alpha=0.65, label=path, c=colors_by_path.get(path, "grey"))
    axes[0].axvline(0, color="black", lw=0.6, alpha=0.4)
    axes[0].axhline(0, color="black", lw=0.6, alpha=0.4)
    axes[0].set_xlabel("PC1: latency / tail burden")
    axes[0].set_ylabel("PC2: availability / loss mode")
    axes[0].set_title("Condition-level PCA scores")
    axes[0].legend(frameon=False)

    names = [r["outcome_feature"].replace("_", " ") for r in loadings]
    vals = [r["PC1_loading"] for r in loadings]
    axes[1].barh(names[::-1], vals[::-1], color="#4063D8")
    axes[1].axvline(0, color="black", lw=0.7)
    axes[1].set_xlabel("PC1 loading")
    axes[1].set_title("What the dominant outcome mode contains")
    fig.tight_layout()
    fig.savefig(OUT / "pca_outcome_modes.png", dpi=220, bbox_inches="tight")
    plt.close(fig)

    grouped = defaultdict(list)
    for c in contrasts:
        grouped[c["factor"]].append(c)
    cats = list(grouped)
    fig, axes = plt.subplots(1, 2, figsize=(12.8, 5.2), sharey=True)
    palette = plt.get_cmap("tab10")
    for y, cat in enumerate(cats):
        ratio_vals = [fnum(c["p95_ratio"]) for c in grouped[cat]]
        ratio_vals = [x for x in ratio_vals if math.isfinite(x) and x > 0]
        success_vals = [fnum(c["success_change_pp"]) for c in grouped[cat]]
        success_vals = [x for x in success_vals if math.isfinite(x)]
        color = palette(y % 10)
        if ratio_vals:
            axes[0].scatter(ratio_vals, np.full(len(ratio_vals), y), s=30, alpha=0.65, color=color)
            axes[0].plot([np.median(ratio_vals)] * 2, [y - 0.25, y + 0.25], color="black", lw=2)
        if success_vals:
            axes[1].scatter(success_vals, np.full(len(success_vals), y), s=30, alpha=0.65, color=color)
            axes[1].plot([np.median(success_vals)] * 2, [y - 0.25, y + 0.25], color="black", lw=2)
    axes[0].axvline(1, color="black", ls="--", lw=0.8)
    axes[0].set_xscale("log")
    axes[0].set_yticks(range(len(cats)), cats)
    axes[0].set_xlabel("p95 RTT ratio (after / before; log scale)")
    axes[0].set_title("Tail effect among accepted replies")
    axes[1].axvline(0, color="black", ls="--", lw=0.8)
    axes[1].set_xlabel("Accepted-delivery change (percentage points)")
    axes[1].set_title("Availability effect across all attempts")
    for ax in axes:
        ax.grid(axis="x", alpha=0.2)
    fig.suptitle("Matched contrasts expose latency and availability as different effects")
    fig.tight_layout()
    fig.savefig(OUT / "matched_contrast_effects.png", dpi=220, bbox_inches="tight")
    plt.close(fig)

    if spatial_bins:
        fig, ax = plt.subplots(figsize=(8.8, 4.4))
        for run in sorted({r["run"] for r in spatial_bins}):
            sub = [r for r in spatial_bins if r["run"] == run and r["attempts"] >= 10]
            ax.plot(
                [(r["distance_bin_start_m"] + r["distance_bin_end_m"]) / 2 for r in sub],
                [100 * r["success_rate"] for r in sub], marker="o", label=run.split("_")[-1],
            )
        ax.set_ylim(-2, 102)
        ax.set_xlabel("Distance bin from RSU marker (m; spatial context, not RF strength)")
        ax.set_ylabel("Accepted custom echoes (%)")
        ax.set_title("PC5 route availability varies across space and runs")
        ax.grid(alpha=0.25)
        ax.legend(frameon=False, title="Collection")
        fig.tight_layout()
        fig.savefig(OUT / "pc5_spatial_availability.png", dpi=220, bbox_inches="tight")
        plt.close(fig)


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    conditions = collect_regular_5g() + collect_airspan() + collect_pc5()
    # Stable ordering makes diffs reproducible.
    conditions.sort(key=lambda r: (r["path"], r["family"], r["run"], r["condition"]))
    write_csv(OUT / "condition_level_metrics.csv", conditions)

    loadings, scores, explained = pca_analysis(conditions)
    write_csv(OUT / "pca_loadings.csv", loadings)
    write_csv(OUT / "pca_condition_scores.csv", scores)

    contrasts = build_contrasts(conditions)
    write_csv(OUT / "matched_contrasts.csv", contrasts)

    spatial_bins = pc5_spatial_bins()
    write_csv(OUT / "pc5_mobility_spatial_bins.csv", spatial_bins)

    rankings, weights = factor_ranking()
    ranking_fields = [
        "rank", "factor", "weighted_score_100", "evidence_grade",
        "observed_severity", "repeatability", "evidence_breadth",
        "identification_strength", "complex_cav_relevance",
        "rank1_probability", "top3_probability", "sensitivity_min_rank",
        "sensitivity_max_rank", "reason",
    ]
    write_csv(OUT / "factor_ranking.csv", rankings, ranking_fields)

    bench = benchmark_summary()
    summary = {
        "analysis_unit": "one condition or one valid Airspan sample",
        "condition_count": len(conditions),
        "condition_count_by_path": {p: sum(r["path"] == p for r in conditions) for p in sorted({r["path"] for r in conditions})},
        "attempts_described": int(sum(r["attempts"] for r in conditions)),
        "pca_eligible_conditions": len(scores),
        "pca_features": [r["outcome_feature"] for r in loadings],
        "pca_explained_variance": [float(x) for x in explained[:3]],
        "ranking_weights": weights,
        "ranked_factors": rankings,
        "benchmarks": bench,
        "pc5_spatial_bin_rows": len(spatial_bins),
        "directional_host_throughput": {
            "car_to_edge_mbps": 13.118, "edge_to_car_mbps": 25.0,
            "target_mbps_each_direction": 25.0,
            "sources": [
                "results/real_5g/20260805_airspan_r1_run_1/load/v2_car_client.csv",
                "results/real_5g/20260805_airspan_r1_run_1/load/v3_edge_client.csv",
            ],
            "boundary": "Host-side directional throughput; not a causal TDD estimate.",
        },
        "restart_success_gap_ms_range": [11714.7, 15100.7],
        "excluded_or_unrankable": [
            "PC5 RSSI/SNR/RSRP/RSRQ, MCS, BLER, retransmissions, and resource-block use: not logged",
            "the latency-derived PC5 mobility signal score: circular with the outcomes",
            "TDD causal effect: no matched same-cell baseline with contemporaneous counters",
            "network-enforced 5QI effect: both observed host captures used TOS 0x0",
            "weather: confounded with date and location",
            "mobility speed effect: confounded with route position and run",
            "independent-UE density: multiclient experiments use logical clients on one UE",
        ],
    }
    (OUT / "analysis_summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    make_figures(rankings, loadings, scores, contrasts, spatial_bins)

    print(json.dumps({
        "conditions": len(conditions), "attempts": summary["attempts_described"],
        "pca_eligible": len(scores), "pc1_variance": explained[0],
        "ranking": [(r["rank"], r["factor"], round(r["weighted_score_100"], 1)) for r in rankings],
        "output": str(OUT),
    }, indent=2))


if __name__ == "__main__":
    main()
