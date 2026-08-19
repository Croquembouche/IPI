#!/usr/bin/env python3
"""Record GL.iNet modem signal history without persisting UI credentials."""

import argparse
import datetime as dt
import json
import pathlib
import signal
import sys
import time

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from selenium.webdriver.support import expected_conditions as expected
from selenium.webdriver.support.ui import WebDriverWait


STOP = False


def request_stop(_signum, _frame):
    global STOP
    STOP = True


def utc_now():
    return dt.datetime.now(dt.timezone.utc).isoformat()


def start_driver(geckodriver, log_path, credentials):
    options = Options()
    options.add_argument("-headless")
    driver = webdriver.Firefox(
        service=Service(geckodriver, log_output=str(log_path)), options=options
    )
    wait = WebDriverWait(driver, 20)
    driver.set_page_load_timeout(30)
    driver.get(credentials["url"])
    if "#/login" in driver.current_url:
        password = wait.until(
            expected.presence_of_element_located((By.CSS_SELECTOR, "input[type=password]"))
        )
        password.send_keys(credentials["password"])
        password.send_keys(Keys.ENTER)
        wait.until(lambda current: "#/login" not in current.current_url)
    driver.get(credentials["url"])
    wait.until(
        lambda current: current.execute_script(
            """
            for (const element of document.querySelectorAll('*')) {
              const component = element.__vue__;
              if (component && component.$options &&
                  component.$options.name === 'modemsignallog' &&
                  component.signals && component.signals.length) return true;
            }
            return false;
            """
        )
    )
    return driver


def read_signals(driver):
    return driver.execute_script(
        """
        for (const element of document.querySelectorAll('*')) {
          const component = element.__vue__;
          if (component && component.$options &&
              component.$options.name === 'modemsignallog') {
            return JSON.parse(JSON.stringify(component.signals || []));
          }
        }
        return [];
        """
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--credentials", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--browser-log", required=True)
    parser.add_argument("--poll-seconds", type=float, default=10.0)
    parser.add_argument("--geckodriver", default="/snap/bin/geckodriver")
    args = parser.parse_args()
    if args.poll_seconds <= 0:
        parser.error("--poll-seconds must be positive")

    credential_path = pathlib.Path(args.credentials)
    credentials = json.loads(credential_path.read_text(encoding="utf-8"))[
        "modem_signal_ui"
    ]
    output_path = pathlib.Path(args.output)
    browser_log = pathlib.Path(args.browser_log)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    browser_log.parent.mkdir(parents=True, exist_ok=True)

    seen = set()
    if output_path.is_file():
        for line in output_path.read_text(encoding="utf-8").splitlines():
            try:
                item = json.loads(line)
                seen.add(
                    (
                        item.get("router_timestamp"),
                        item.get("network_type"),
                        item.get("sim_slot"),
                    )
                )
            except json.JSONDecodeError:
                pass

    driver = None
    with output_path.open("a", encoding="utf-8", buffering=1) as output:
        while not STOP:
            try:
                if driver is None:
                    driver = start_driver(args.geckodriver, browser_log, credentials)
                observed_at = utc_now()
                for item in read_signals(driver):
                    key = (item.get("timestamp"), item.get("network_type"), item.get("slot"))
                    if key in seen:
                        continue
                    record = {
                        "schema": "edge4av-glinet-signal-sample-v1",
                        "observed_at": observed_at,
                        "router_timestamp": item.get("timestamp"),
                        "network_type": item.get("network_type"),
                        "mode": item.get("mode"),
                        "sim_slot": item.get("slot"),
                        "strength": item.get("strength"),
                        "rsrp_dbm": item.get("rsrp"),
                        "rsrq_db": item.get("rsrq"),
                        "sinr_db": item.get("sinr"),
                    }
                    output.write(json.dumps(record, sort_keys=True) + "\n")
                    seen.add(key)
            except Exception as error:  # Keep acquisition alive across UI restarts.
                print(
                    f"[{utc_now()}] modem signal collection error: "
                    f"{type(error).__name__}: {error}",
                    file=sys.stderr,
                    flush=True,
                )
                if driver is not None:
                    try:
                        driver.quit()
                    except Exception:
                        pass
                    driver = None
            deadline = time.monotonic() + args.poll_seconds
            while not STOP and time.monotonic() < deadline:
                time.sleep(min(0.2, deadline - time.monotonic()))

    if driver is not None:
        driver.quit()


if __name__ == "__main__":
    signal.signal(signal.SIGINT, request_stop)
    signal.signal(signal.SIGTERM, request_stop)
    main()
