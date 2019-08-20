/* DEBUG
 * Sound turns noise when no gui elements present !?
 *  - solved if v-sync is disabled
 *
 *
 * more or less cryptic crashes
 * - 4.3.2019 changed instrument to piano 3
 * - 2.3.2019 started and stopped playing while changing instument of instrument track currently playing in the loop sequencer
 * - 19.1.2019 added multidelay
 * - 11.1.2019 changed voice waveshape from wave7 to wave6, had one modulator, biquad filter and multidelay
 * - 16.12.2018 tried to change modulator amount from 4 to 3
 * - 15.12.2018 tried to set envelope panel visible
 * - 15.12.2018 tried to add biquad filter
 * - 14.12.2018 removed biquad filter while associated gui visible
 * - 13.12.2018 tried to set modulator panel visible
 * - 5.12.2018 changed instrument to Accordion 1 (intensive playing simultaneously)
 * - 26.11.2018 crashed when removed multidelay
 * - 23.11.2018 crash when applied pad (piano) preset, had biquad filter active (maybe division by zero)
 * - 15.11.2018 crash; played through four loops couple of times, and crash soon after increasing max notes above 80
    (four instruments, one AM, one or more delaylines), it sounded like some audio buffer was overflowing, beeps and stuff before crash
    perhaps should clip audiowave above certain amplitude (1.0)?
 * - 13.11.2018 crash when changed AM waveform from saw to testing01
 * - 13.11.2018 crash when changed oscillator from Voice 1 to Voice 2 (had two AMs and multidelay with the instrument)
 *   - probably GUI bug againplayNote
 * - 13.11.2018 changed AHE envelope attack to zero and crash (division by zero somewhere?)
 * - 12.11.2018 working pretty robustly, mostly, but several crashes has appeared with the sequencer and multiple instruments
 *     - during adding of max number of notes
 *     - changed oscillator from gradient noise to something else
 * - involved unnderruns and stream stop() and/or start(), happened 6x
 * - a big typo corrected in NumberBox class, probably most gui-related bugs solved
 *    - changed waveform (had three voices, one of gradient noise), happened 1x
 *    - changed oscillator, happened 8x
 * - after doing little bit more, crash tend to happen when removing an oscillator, probably GUI bug
 *
 * halted when backspaced AHE envelope hold exponent numberbox (probably unmanageable number occurred)
 *  - apparently a really long loop because the number momentarily takes the max value before clearance
 * holding notes get stuck on shader
 */

/* TODO/FIXME
 * Recorded track notes not working
 * Voice cross modulation matrix 16x16 (DONE)
 * Audio recording tracks into loop mode (DONE)
    - loop region selection from recording
    - edit mode for recordings (copy, paste, cut, delete, move, combine...)
 * Debugging system needed. Everything non-trivial into log files.
 * Use visibility instead of removing/recreaing to control the visiblity of main synth panels (mostly DONE)
 * Record note samples at two instances: during synthesizing, or before hand in different threads
 *  - limit the amount of threads, take care of the threads after recording finished
 *
 * Unison
 *  - controllable phase
 *
 * Modulators
 *  - parameters able to be modulated
 *    - voice: stero offset, pan, unison detuning, unison phase
 *  - all modulators with fixed output range [0, 1]
 *    - scaling controlled by the variables at the receiving end
 * Effects
 * - zita verb: diffusion value 0.99 unstable
 * FILE I/O
 *  - faster file parsing (DONE!)
 *  - generalize functions (partially done!) (completely DONE!)
 *  - save/load sequences
 * Truncated spectrums might be better if value would be the max value within the range than the sum
 * Recover sound when things go mute 
 * Effect matrix panel (effects modular and assignable to individual voices and modulators?)
 * 1 Hz high-pass filter active all time (?)
 * Envelopes
 *  - 4-point and 5-point envelope not alright
 *  - PAD envelope could be symmetric
 * Go fix line rendering...
 * Other synth parameters controllable my midikeyboard (keys and other knobs) than voice
 *  - filter frequency
 *  - filter bandwidth
 *  - modulator frequency
 *  - modulator depth
 *  - (This will be cool feature. It would be fun to have e.g. a rich pad sound playing with lot of harmonics and stuff,
 *     and play tones within tones with multiple midikeyboards)
 *  - before (or instead if) this, biquad filters for every voice, and every note and optionable frequency relative to the note frequency (DONE)
 * get rid of oscillator set class, amplitude modulator and frequency modulator, and implement chainable modulator system
 * for as many parameters as possible
 *  - lot of hard work today, but I think this should be worth it (DONE!)
 *  - took two days. Worthy of two weeks of hard work. At least. This is too much. These are the moments when you can almost
 *    touch the eternity.
 *  - Perhaps rid of also the new GenericModulator class and Voice, and a common Oscillator class instead and every oscillator present in the matrix
 * prevent too big FFT sizes
 * Visualization of filter effect (DONE)
 *  - a sample containing all the frequencies (acquired through reverse FFT?) through the filter and transformation (back) to frequency domain
 * Different synthetisizers
 *  - abstractify voice class, and inherit the synths from it
 *  - the current additive synth
 *  - substractive synth (noise filtered with band-pass filters)
 *     - might be really good for flute like sounds such as organ
 * detect when filters go out of equilibrium and fix it automatically without needed to set inactive and active again
 * Finish biquad filter
 * - cascade doesn't work
 * Clipping of out of bounds values. Get rid of possible divisions by zero
 *  - inapproriate values for sound output sometimes disable the sound for good
 * Console input grab
 *  - Currently text input of every gui element is directed to console.
 * automatically recognise sound server, do something else than halt if incorrect sound server is on (DONE)
 * automatically connect midi controllers (?)
 * Presets
 *  - oscillator set presets
 *  - instrument presets
 *  - effect presets
 * Gui
 *  - unintentionally wide gap sometimes in NumberBox (TextBox) between label and value
 *  - internal panel for voice (and AM and FM if not getting rid of those)
 *  - better automatic panel positioning
 *  - encapsulate panel related stuff into classes (mostly DONE)
 *  - partial visualization
 *  - waveform rendering for either active oscillator, oscillator set or instrument (whole instrument waveform not yet visualized)

 * Midi
 *  - sepearate thread for sending midi out events from sequencer
 * Effects and filters
 * - global effect unit with variable number of multidelays and other effects to come
 * - local effect units for instruments/oscillator sets/voices (DONE)
 * Synth
 *  - annoying clicks occur when waveform starting amplitude is non-zero amd envelope starts at full volume
 *    - maybe automatic interpolation from zero amplitude to the waveform start amplitude for every oscillator
 *    - or automatic phase adjustment to zero point of curves
 *  - annoying clicks occur when start time matches exactly paTime in the following, apparently.
 *    Not an actual problem currently, but there it is ruining the integrity in the background
 *     - bufferCount = (long)(t * sampleRate / framesPerBuffer);
 *       paTime = bufferCount * framesPerBuffer / sampleRate;
 *     - might be division by zero somewhere... few aborts associated with those kind of notes
 *  - new oscillator class: composite voice to deal with pitch-dependent timbre transitions on a single instrument
 *    - another option would be to have a similar composite instrument as drum pad but interpolating the notes between the designated notes
 *  - long wavetables (with multiple cycles)
 *     - especially for gradient noise
 *     - possibly for combining non-integer partials
 *     - and maybe unison too
 *     - would be ineresting to try to form composite wave of several cycles and FFT it forward and back again,
 *       it might be one long continuous cycle
 *  - customizable wavetable size
 * Modulation
 *  - frequence modulator probably should handle each partial seperately
 * more detuning/randomness for higher partials
 * randomness and variation into partial series
 *  - e.g. modulation between two or more presets
 *  - random fluctuation between min and max gain values for each partial rather than fixed value
 *   - this effect can be achieved with unison detuning already, but not in a controlled way...
 * partial series dependent on pitch
 *  - e.g. partial presets for the first notes of each octave and interpolation for other notes
 *  - algorithmic (higher pitches seem to have shorter series)
 * the pad synthesis algorithm in zynsubaddfx seems pretty well suited for achieving most of the
 *   things I am attempting to do, and it would be so much faster in many cases
 *    - e.g. instead of generating one whole wavetable, it could be used to produce those naturally
 *      random and detuned higher partials
 * mono not working
 * extended waveform modification stuff
 *  - modulation
 *  - hand drawing
 *  - use currently visible waveform as base waveform
 *  - option to smoothen transition from cycle end to cycle begin (kind of low-pass filter)
 * optimization for performance (e.g. intermediate buffers)
 *   - not much I could do to improve envelope performance
 *      - observations based on how many notes could be played simultaneously without unnderruns occurring
 *      - up to 7 % more notes by replacing math operations in envelope with lookup tables but without
 *        interpolation, sound quality sucks (smaller luts yield faster but worse sounding stuff)
 *      - up to 6 % more notes with two two-dimensional lookup-tables for (t and holding time axis)
 * remove playing notes as soon as envelope goes near zero
 * custom partial factor patterns (DONE)
 *  - more patterns!
 *  - hand drawn gain and factor patterns
 *  - piano inharmonicity
 * waveform instead of wave form
 * partial series presets for various instruments
 *  - more needed!
 * the main problem is still unsolved, smooth loop changes, new paradigm needed perhaps (SOLVED!, perhaps)
 *  - this was the reason in the spring I got disspirited about this synth stuff. I think, now it is finally done.
 * underruns cause a lot of trouble, robustness needed
 * shader don't know yet how to visualise all the waveforms
 *  - perhaps visualization based on delay line
 * Own gui panel for waveform preview(?)
 * put absolutely private stuff into private category
*
 */
