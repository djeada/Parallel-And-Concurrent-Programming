#!/usr/bin/env python3
"""
Script to validate that all links in markdown files are absolute GitHub URLs.
This prevents relative links from being accidentally introduced.
"""

import re
import sys
from pathlib import Path
from typing import List, Tuple

def find_markdown_links(content: str, filename: str) -> List[Tuple[int, str, str]]:
    """
    Find all markdown links in content.
    Returns list of (line_number, link_text, link_url) tuples.
    """
    links = []
    lines = content.split('\n')
    
    # Pattern to match [text](url) markdown links
    link_pattern = re.compile(r'\[([^\]]+)\]\(([^)]+)\)')
    
    for line_num, line in enumerate(lines, 1):
        # Skip code blocks (lines with ```python```, etc.)
        if '```' in line:
            continue
            
        for match in link_pattern.finditer(line):
            link_text = match.group(1)
            link_url = match.group(2)
            links.append((line_num, link_text, link_url))
    
    return links


def is_relative_link(url: str) -> bool:
    """
    Check if a URL is a relative link that should be absolute.
    Returns True if the link is relative and points to repository content.
    """
    # Ignore anchors (internal page links)
    if url.startswith('#'):
        return False
    
    # Ignore empty links
    if not url or url == '()':
        return False
    
    # Ignore absolute URLs (http, https, mailto, etc.)
    if url.startswith(('http://', 'https://', 'mailto:', 'ftp://')):
        return False
    
    # Check for relative paths that point to repository content
    relative_patterns = [
        '../',  # Parent directory references
        'src/',  # Source code references
        'scripts/',  # Script references  
        'notes/',  # Notes references
        './src/',  # Current directory + src
        './scripts/',  # Current directory + scripts
        './notes/',  # Current directory + notes
    ]
    
    return any(url.startswith(pattern) for pattern in relative_patterns)


def validate_markdown_file(filepath: Path) -> List[Tuple[int, str, str]]:
    """
    Validate a markdown file for relative links.
    Returns list of (line_number, link_text, link_url) for relative links found.
    """
    try:
        content = filepath.read_text(encoding='utf-8')
    except Exception as e:
        print(f"Warning: Could not read {filepath}: {e}", file=sys.stderr)
        return []
    
    all_links = find_markdown_links(content, str(filepath))
    relative_links = [
        (line_num, text, url) 
        for line_num, text, url in all_links 
        if is_relative_link(url)
    ]
    
    return relative_links


def main():
    """Main validation function."""
    repo_root = Path(__file__).parent.parent
    notes_dir = repo_root / 'notes'
    
    if not notes_dir.exists():
        print(f"Error: Notes directory not found: {notes_dir}", file=sys.stderr)
        return 1
    
    # Find all markdown files in notes directory
    markdown_files = list(notes_dir.glob('*.md'))
    
    if not markdown_files:
        print(f"Warning: No markdown files found in {notes_dir}", file=sys.stderr)
        return 0
    
    print(f"Validating {len(markdown_files)} markdown files in notes/...\n")
    
    all_issues = []
    for md_file in sorted(markdown_files):
        relative_links = validate_markdown_file(md_file)
        if relative_links:
            all_issues.append((md_file, relative_links))
    
    if all_issues:
        print("❌ Found relative links that should be absolute:\n")
        for filepath, links in all_issues:
            print(f"  {filepath.relative_to(repo_root)}:")
            for line_num, text, url in links:
                print(f"    Line {line_num}: [{text}]({url})")
            print()
        
        print(f"Total: {sum(len(links) for _, links in all_issues)} relative link(s) found.")
        print("\nAll links in markdown files should use absolute GitHub URLs like:")
        print("https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/...")
        return 1
    else:
        print("✅ All links in notes/ are absolute GitHub URLs!")
        print(f"   Validated {len(markdown_files)} markdown file(s).")
        return 0


if __name__ == '__main__':
    sys.exit(main())
