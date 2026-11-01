# Embedded integration design (simulation only)

```text
                         USB (host link)
Host PC ─────────────── Mock MCU ── SPI/UART ── Mock FPGA
   │                                             │
   └── USB/JTAG probe ───── JTAG TCK/TMS/TDI/TDO ┘
```

All endpoints are software mocks. No board, pin, voltage, or JTAG signaling was exercised.

| Link | Signals | Example domain | Notes |
|---|---|---:|---|
| Host to MCU | USB CDC | USB 5 V input; board regulates locally | Use board vendor limits; do not feed 5 V GPIO into 3.3 V pins. |
| MCU to FPGA | SPI SCLK/MOSI/MISO/CS or UART TX/RX | Assume 3.3 V CMOS on both sides | Verify actual board I/O-bank voltage; add a bidirectional level shifter where domains differ. |
| JTAG probe to FPGA | TCK/TMS/TDI/TDO, optional TRST | Match FPGA bank/probe I/O voltage | The probe owns JTAG during programming/debug. Do not drive these nets from firmware simultaneously. |
| Power/reference | GND common reference; separately regulated rails | Board-specific | Join signal grounds; avoid tying independent supply outputs together. Observe current and sequencing limits. |

Mock adapter lifecycle: discover capabilities, connect, emit one temperature telemetry record, simulate a recoverable timeout, then expose retry/disconnect. The UI and CLI label this as simulated behavior. Device pin assignments are intentionally not asserted for unspecified hardware models.
