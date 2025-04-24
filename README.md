# tosh

`tosh` is a toy shell which used to training my cplusplus programming skill.

TODOS:

- [x] `terminal/events` subsystem

  - `EventReader` class, supports `poll(pred, timeout)` and `read(pred, timeout)` method
  - `Event` class and `EventType` enum (GETCURSOR, ASCII, etc)
  - Rename `ANSIPort` class to `Terminal` and rewrite it
  - Finish `Document` class (which implements user side terminal operations)

- [ ] Environment variable support
- [ ] (Maybe) History support
- [ ] (Maybe) Configuration support

## Bugs

- `EventReader` is not a reliable event reader, if user's input is too fast, it will miss some events, which may cause unexpected behavior. I cannot fix this because I have no idea how to fix it.
