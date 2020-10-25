package mfc.autopy;

import mfc.automator.Automaton;

public class Entrance {
    public static void main(String[] args) {
        // Used to load the 'bridge' library on application startup.
        System.loadLibrary("bridge");

        try {
            new Automaton().runPython("main.py");
        } catch (Exception err) {
            err.printStackTrace();
        }
    }
}
