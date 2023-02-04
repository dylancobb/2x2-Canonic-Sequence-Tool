# canonic-sequences
Generates canonic sequences, provides a means to search them by characteristics, and a tool to generate
formatted LaTeX files with notated musical examples in LilyPond for my book

Written in C. Run 'make' from the directory to build the program: instructions for usage are given within the program itself.

Canonic sequences are musical objects. A quick summary:
If a melodic idea is repeated, that is "repetition".
If it is repeated, but transposed to another scale degree, that is a "sequence".
If material in one voice is repeated, but in a different voice, that is "imitation".
If the repetition is maintained in strict fashion (i.e. not changing any intervals or rhythms except by uniform mathematical
means), the imitation is said to be "canonic".
If a melody which is itself a sequence is imitated canonically, a "canonic sequence" is formed.

Obviously we cannot trivially copy and paste any string of notes to create a "valid" canonic sequence: they must be melodically valid,
and they must also form correct "counterpoint". This algorithm uses simple models consisting of two melodic intervals and two voices.
Currently it generates all possible 2z2 diatonic sequences, with chromatic sequences and 3x3 + 4x4 sequences planned soon.

The search criteria like "Jv", "entry intervals", "voice pair applicability" etc. are a little beyond the scope of this readme to explain,
and refer to features that govern their use in actual musical contexts, mostly derived from the work of the composer/theorist Sergei Taneyev.
