CS 485 — Minimal Sandbox Test Suite
====================================================================

This folder contains small C test programs for testing the sandbox project.

Each test prints INFO messages and exits with:
  0 = PASS
  1 = FAIL (policy/leakage issue)
  2 = ERROR (unexpected runtime error)

Folders
-------
public/milestone1   : Public tests for Milestone 1 (10 programs)
public/milestone2   : Public tests for Milestone 2 (10 programs)
public/final        : Public tests for Final Demo (10 programs)

Build
-----
  make