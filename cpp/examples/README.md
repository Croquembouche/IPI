# Example Apps Layout

- `library/` – developer utilities and sanity checks that exercise the core IPI
  library without hardware: `build_service_request`, `v2x_roundtrip`.
- `device/` – binaries that interface with V2X hardware/ROS2: `mocar_ipi_demo`
  (Mocar SDK), `ros2_bsm_broadcaster` (ROS2→Mocar rebroadcast).

Notes:
- `mocar_ipi_demo` supports runtime SPaT overrides via a plain text file set in
  `MOCAR_SPAT_FILE` (lines: `<signal_group> <phase_state_int> <likely_time_ms>`);
  edits take effect on the next transmit tick without restart.

Build with `-DIPI_BUILD_EXAMPLES=ON`; enable hardware/ROS2-specific ones via
`IPI_ENABLE_MOCAR_EXAMPLES` and `IPI_ENABLE_ROS2_BRIDGE` as needed.
