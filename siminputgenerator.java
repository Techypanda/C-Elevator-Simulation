/*
    NOT TO BE ASSESSED, THIS IS TO GENERATE A RANDOM SIM_INPUT
*/
import java.util.Random;
import java.io.*;
/*******************************************************************************
*   PURPOSE: NOT TO BE ASSESSED: This will simply generate a sim_input file.
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
public class siminputgenerator {
    public static void main(String[] args) {
        FileOutputStream out;
        PrintWriter pw;
        try {
            out = new FileOutputStream("sim_input");
            pw = new PrintWriter(out);
            Random rand = new Random();
            int n = rand.nextInt(50) + 50;
            System.out.printf("Generating %d inputs.%n", n);
            for (int i = 0; i < n; i++) {
                pw.printf("%d %d%n",rand.nextInt(20)+1,rand.nextInt(20)+1);
            }
            pw.close();
            out.close();
            System.out.println("Successfully Generated sim_input");
        } catch(IOException e) {
            System.out.println(e.getMessage());
        }
    }
}
