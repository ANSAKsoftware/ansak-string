ansak-string
============

Well, look at that. I talk about what I'm doing for myself and it gets used at work in one small place.
And then in another.
And then in another.

The long and the short of it is, I need a single place for nothing but my simple string libraries.

It's all about purifying input. If your string (whether it's 7-bit ASCII, UTF-8, UTF-16, UCS-2 or UTF-32/UCS-4) makes it through my APIs (into 7-bit ASCII, UTF-8 etc.), then that's what it is. No more, no less.

Additional stuff:

* unicodeLength gives number of UCS-4 codes (regardless of composite/composable points) in a string
* split and join, like python's string and array methods

To come:

* all-Unicode-sensitive "tolower"
* stringprep style purifying?
* maybe punycode encoding/decoding too? Not sure about that one
* normalizing to composed or to consitituent-parts strings? more likely than punycode
