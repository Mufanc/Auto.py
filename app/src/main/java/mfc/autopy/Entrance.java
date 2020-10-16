package mfc.autopy;

import mfc.automator.Automaton;

public class Entrance {
    // Used to load the 'bridge' library on application startup.
    static {
        System.loadLibrary("bridge");
    }

    public static void main(String[] args) {
        new Automaton().uiTest2();
    }
}
