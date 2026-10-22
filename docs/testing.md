# Week 1 — Smart Grid Testing Report

## 1. Test Environment

- Programming language: C
- Compiler: GCC
- Interface: Command Line Interface
- Execution: Offline
- Operating system: Windows

## 2. Test Cases

| Test ID | Scenario | Expected Result | Actual Result |
|---|---|---|---|
| T01 | Initialize empty graph | 0 nodes, 0 edges | To verify |
| T02 | Insert valid node | Success | To verify |
| T03 | Insert duplicate node | Rejected | To verify |
| T04 | Insert valid edge | Success | To verify |
| T05 | Invalid edge endpoint | Rejected | To verify |
| T06 | Load sample network | 9 nodes, 8 edges | To verify |
| T07 | Build adjacency list | 16 adjacency entries | To verify |
| T08 | Fail Hospital line | 20 MW disconnected | To verify |
| T09 | Fail Substation A supply | 45 MW disconnected | To verify |
| T10 | Fail Substations A and B supply | 75 MW disconnected | To verify |
| T11 | Restore failed lines | 0 MW disconnected | To verify |
| T12 | Invalid network file | Rejected safely | To verify |
| T13 | Complete integration workflow | All assertions pass | To verify |

## 3. Acceptance Criteria

Week 1 is considered complete when:

- The application compiles without warnings.
- The CLI runs correctly.
- The sample network loads successfully.
- The edge list and adjacency list remain consistent.
- Invalid inputs are rejected.
- Transmission-line failures are reflected in network analysis.
- Restoration returns the expected connectivity.
- All automated tests pass.

## 4. Known Limitations

- No capacity-constrained power allocation.
- No optimized power routing.
- No automatic restoration decisions.
- No electrical load-flow calculations.

## 5. Final Test Result

Pending final execution and verification.