# Contributing to Smart FSM Access Terminal

Contributions, bug reports, and feature requests are welcome!

## How to Contribute

1. **Fork the repository** on GitHub.
2. **Create a feature branch:** `git checkout -b feature/amazing-feature`
3. **Commit your changes:** `git commit -m "Add amazing feature"`
4. **Push to branch:** `git push origin feature/amazing-feature`
5. **Open a Pull Request** against the `main` branch.

## Guidelines

- **Hardware modifications:** Update `hardware_wiring_guide.md` and `dsd_project_documentation.md` with new schematics and pin mappings.
- **Firmware updates:** Ensure Arduino code compiles without warnings and runs on standard NodeMCU 1.0 (ESP-12E).
- **FSM changes:** Run `node test_fsm_engine.js` to verify zero regressions across all 16 input combinations.
- **Documentation:** Keep Markdown files well-structured with clear tables and code blocks.

## Reporting Bugs

Please open an issue on GitHub with:
- Steps to reproduce the bug
- Expected vs actual behavior
- Hardware setup details (if applicable)
- Serial monitor logs / console output