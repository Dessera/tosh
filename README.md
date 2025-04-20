# tosh

`tosh` is a toy shell which used to training my cplusplus programming skill.

TODOS:

- [ ] `terminal/events` subsystem

  - `EventReader` class, supports `poll(pred, timeout)` and `read(pred, timeout)` method
  - `Event` class and `EventType` enum (GETCURSOR, ASCII, etc)
  - Rename `ANSIPort` class to `Terminal` and rewrite it
  - Finish `Document` class (which implements user side terminal operations)

- [ ] Environment variable support
- [ ] (Maybe) History support
- [ ] (Maybe) Configuration support
