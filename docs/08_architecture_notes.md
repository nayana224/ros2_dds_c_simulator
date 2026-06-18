# Project Architecture Notes

## Why this note exists

When building a project, it is easy to focus only on "making it work."
That is normal.

But as a project grows, another skill becomes important:

- how to divide the code into understandable parts
- how to make each file responsible for one kind of work
- how to avoid writing code that becomes hard to read or change

This note is a beginner-friendly summary of those ideas.
It is written so it can be copied into Notion as study notes.

---

## 1. What "architecture" means in a small project

In a student project, architecture does not mean something huge or fancy.

It usually means:

- what files exist
- what each file is responsible for
- how data flows through the program
- which part is allowed to call which other part

Good architecture makes the project:

- easier to understand
- easier to debug
- easier to test
- easier to modify later

Bad architecture usually still works at first, but becomes painful when:

- a new feature is added
- one output format changes
- one bug fix breaks another area
- the code becomes too long to read comfortably

---

## 2. Core idea: Separation of Concerns

This is one of the most important ideas in software design.

It means:

> Different kinds of work should be handled in different places.

For example, these jobs are different jobs:

- reading user input
- validating values
- storing data
- changing data structures
- printing results
- searching a graph

If all of these are mixed inside one big function or one big file, the code becomes hard to reason about.

### Simple example

Bad direction:

- one function reads input
- parses command
- changes data
- prints output
- frees memory

Better direction:

- one function reads input
- another function parses it
- another function performs the real logic
- another function prints the result

This is easier to follow because each part has a clearer role.

---

## 3. Core idea: Single Responsibility

This idea is closely related.

It means:

> A file or function should have one main reason to change.

### Example

Suppose one file handles:

- topic registration
- message queue ordering
- BFS path search
- console printing

That file may need to change for many unrelated reasons:

- because queue logic changed
- because graph logic changed
- because output format changed
- because input behavior changed

That is a sign that responsibility is too broad.

A better design is to separate those responsibilities.

---

## 4. Core idea: Layered Architecture

Layered architecture means the program is divided into levels.

Each level has a different role.

For a small C project, this is a very practical version:

1. UI layer
2. Core logic layer
3. Data structure/storage layer

### 1. UI layer

This layer talks to the user.

Examples:

- CLI input
- menu display
- help text
- printing messages like "success" or "failed"

This layer should know:

- command names
- output text
- user interaction rules

This layer should not contain the real business logic if possible.

### 2. Core logic layer

This layer decides what the program actually does.

Examples:

- register a node
- publish a message
- receive a message
- search a path

This layer should know the rules of the project.

This layer should not care about:

- whether the user typed a CLI command
- how the result is printed

### 3. Data structure/storage layer

This layer manages how data is stored internally.

Examples:

- linked list operations
- queue insertion/removal
- helper functions for searching
- graph traversal helpers

This layer is often part of the core in a small project, but it is still useful to think of it separately.

---

## 5. Why layering matters

Layering is important because it reduces coupling.

### Coupling

Coupling means how strongly parts of the code depend on each other.

Too much coupling is a problem.

Example:

- if your core logic directly prints to the console
- and later you want a GUI version
- then the core logic must be edited again

That means the logic is tied to the CLI too tightly.

### Good layering gives flexibility

If the CLI layer and the logic layer are separate:

- CLI can change without changing core logic
- output messages can change without touching queue logic
- tests can call logic functions directly

This is one of the biggest real-world benefits.

---

## 6. High Cohesion and Low Coupling

These are two keywords often used in software design.

### High cohesion

Functions inside one file should belong together.

Good example:

- a file only about message queue behavior

Bad example:

- the same file mixes queue code, CLI code, and path printing

### Low coupling

Different files should depend on each other as little as possible.

Good example:

- CLI calls core functions
- core does not know CLI exists

Bad example:

- core functions depend on CLI-specific strings or printing rules

---

## 7. What "too complex" usually looks like

A project is often becoming too complex when:

- one file becomes very large
- one function becomes very long
- one function does several unrelated things
- changing output requires changing core logic
- the same validation code is repeated many times
- understanding one feature requires reading the whole project

Complexity is not just about line count.

A file can be 300 lines and still feel clear.
A file can be 120 lines and still feel confusing.

The real question is:

> Can I understand the responsibility of this file quickly?

If the answer is no, structure likely needs improvement.

---

## 8. Practical rule for beginners

When you do not know how to structure a project, start with this question:

> "What kind of job is this code doing?"

Then group code by job.

Good categories:

- input/output
- state management
- data validation
- algorithms
- formatting/printing
- test code

This is usually a better starting point than grouping code randomly.

---

## 9. Applying the idea to this project

Current project concept:

- `main.c`: program entry
- `cli.c`: command input and command dispatch
- `simulator_lifecycle.c`: simulator initialization and cleanup
- `simulator_registry.c`: node/topic/publisher/subscriber registration
- `simulator_message.c`: message queue and publish/receive logic
- `simulator_graph.c`: graph traversal and output formatting support

This is now much closer to a clean small-project structure.
The earlier single-file simulator core has already been split into smaller
responsibility-based files.

### A more layered version

- `main.c`
  - start program
  - initialize simulator
  - run CLI
  - cleanup

- `cli.c`
  - read command
  - parse arguments
  - call simulator API
  - print user-facing messages

- `simulator_registry.c`
  - add/find node
  - add/find topic
  - add publisher/subscriber

- `simulator_message.c`
  - create message
  - enqueue by priority
  - dequeue
  - publish
  - receive

- `simulator_graph.c`
  - formatting helpers for list/graph/path output
  - BFS path search

- `simulator_internal.h`
  - internal helper declarations shared by simulator source files

- `simulator.h`
  - only public API

This does not make the project "fancier."
It makes the project easier to read.

---

## 10. Very important rule: separate logic from printing

One of the most useful design habits is this:

> Core logic should do the work.
> UI code should present the result.

### Less ideal style

The core directly calls `printf()` and decides the final screen output.

This mixes logic and presentation.

### Better long-term style

The core builds the result.
The CLI prints it.

For example, this project now uses a direction like:

```c
int simulator_format_communication_graph(...);
int simulator_format_path_between_nodes(...);
```

and the CLI prints the final user-facing text.

Why this matters:

- easier testing
- easier reuse
- easier future changes
- cleaner responsibility boundaries

---

## 11. What to do when code becomes too long

Do not split files just because they are "big."
Split them when responsibilities are mixed.

Good reason to split:

- one file contains graph logic and queue logic and console output

Not-so-good reason to split:

- one file is 220 lines but all 220 lines are clearly one responsibility

### A useful rule

Split when:

- the file has multiple mental categories
- or you keep scrolling to understand one feature
- or you keep saying "this part feels unrelated"

---

## 12. How professionals usually think before refactoring

Before changing structure, ask:

1. What is this file responsible for now?
2. What responsibilities are mixed together?
3. Which responsibilities change for different reasons?
4. What can be separated without changing behavior?
5. What tests do I need before moving code?

This is important.

Good refactoring is not random movement.
It is behavior-preserving structural improvement.

---

## 13. Safe refactoring order

When improving structure, a safe order is:

1. Make sure tests exist first.
2. Do not change behavior and structure at the same time.
3. Move one responsibility at a time.
4. Re-run tests after each step.
5. Only then consider API redesign.

This reduces fear and helps avoid large confusing rewrites.

---

## 14. Beginner mistakes to avoid

### Mistake 1: Making everything abstract too early

Not every small project needs many layers and patterns.
Too much abstraction can be just as bad as no structure.

### Mistake 2: Putting all logic into UI code

This makes the code feel easy at first, but hard later.

### Mistake 3: Splitting files randomly

Files should be split by responsibility, not by guesswork.

### Mistake 4: Refactoring without tests

Without tests, it is very easy to accidentally break behavior.

### Mistake 5: Changing too much at once

Large rewrites are hard to understand and hard to review.

---

## 15. A practical checklist for your own projects

Use these questions when reviewing your structure.

### File responsibility checklist

- Can I describe this file in one sentence?
- Does this file have one main reason to change?
- Are there unrelated jobs mixed together here?

### Function checklist

- Does this function do one main thing?
- Is the name consistent with what it really does?
- Is input validation separate enough from the main logic?
- Is it short enough to understand without stress?

### Layering checklist

- Does input/output code know too much about internal logic?
- Does core logic depend on printing or UI details?
- Can I test the logic without running the CLI?

### Maintainability checklist

- If I change output text, do I need to edit core logic?
- If I change the queue rule, do I need to edit CLI code?
- If I add a new UI later, can I reuse the core?

If too many answers are bad, the structure probably needs improvement.

---

## 16. What "good enough" looks like for a student project

You do not need enterprise-level architecture.

For this kind of project, "good enough" usually means:

- entry point is small
- UI/input code is separate
- core logic is separate
- large responsibilities are not all mixed in one file
- tests exist for important behavior
- code is understandable without reading 1000 unrelated lines

That is already a strong result.

---

## 17. Recommended mindset

Do not ask:

> "How do I make this look professional?"

Ask:

> "How do I make this easier to understand, test, and change?"

That question leads to much better architecture decisions.

Professional-looking code is usually the result of clear responsibility boundaries, not fancy patterns.

---

## 18. Suggested next step for this project

Before rewriting the whole codebase, a good next step is:

1. Review current responsibilities by file.
2. Mark which functions belong to:
   - CLI
   - registry
   - message queue
   - graph/BFS
3. Move code gradually, not all at once.
4. Keep tests passing after each move.

This is the safest path.

---

## 19. One-sentence summary

Good project structure means each part of the program has a clear job, changes for a small number of reasons, and depends on other parts as little as possible.
