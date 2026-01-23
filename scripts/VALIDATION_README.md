# Link Validation

This directory contains a validation script to ensure all links in markdown files are absolute GitHub URLs.

## Purpose

The `validate_links.py` script checks that all links in the `notes/` directory use absolute GitHub URLs (e.g., `https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/...`) rather than relative paths (e.g., `../src/`, `src/`, `scripts/`).

## Usage

```bash
# Run from repository root
python3 scripts/validate_links.py

# Or make it executable and run directly
./scripts/validate_links.py
```

## Exit Codes

- `0`: All links are absolute (validation passed)
- `1`: Found relative links that need to be converted to absolute URLs

## What It Checks

The script validates that links in markdown files:
- ✅ Use absolute GitHub URLs for source code references
- ✅ Use absolute GitHub URLs for script references
- ✅ Use absolute URLs for external resources
- ✅ Internal page anchors (e.g., `#section`) are allowed

## Integration

This script can be integrated into:
- Pre-commit hooks
- CI/CD pipelines
- Manual validation before merging PRs

## Example

```bash
$ python3 scripts/validate_links.py
Validating 9 markdown files in notes/...

✅ All links in notes/ are absolute GitHub URLs!
   Validated 9 markdown file(s).
```
