ansak-string
============

This library started as the first piece of a larger project that's gotten somewhat stalled. It's a simple string library.

It's all about purifying input. If your string (whether it's 7-bit ASCII, UTF-8, UTF-16, UCS-2 or UTF-32/UCS-4) makes it through my APIs (into 7-bit ASCII, UTF-8 etc.), then that's what it is. No more, no less.

There's an all-Unicode-sensitive tolower of a string. It can operate in "Turkic" mode if you give it an ISO-639 two or three character code for Turkish, Azerbaizhani, Kazakh or Tatar, I-dot and dotless-i are handled correctly for those languages.

Additional stuff:

* unicodeLength gives number of UCS-4 codes (regardless of composite/composable points) in a string
* split and join, like python's string and array methods
* all-Unicode-sensitive "tolower"

To come:

* stringprep style purifying?
* maybe punycode encoding/decoding too? Not sure about that one
* normalizing to composed or to consitituent-parts strings?
* I'd like to provide these through a variadic template-to-pipeline for maximal efficiency: for most of these kinds of things you have to convert to UCS-4 before you do what you really want.
