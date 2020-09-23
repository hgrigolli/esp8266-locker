package dev.grigolli.arduino;

import java.io.*;
import java.net.*;

public class TCPServer {

    public static void main(String argv[]) throws Exception
    {
        String clientSentence;
        String capitalizedSentence;

        ServerSocket welcomeSocket = new ServerSocket(41821);
        System.out.println("Socket iniciado - " + welcomeSocket.getInetAddress());
        while(true) {

            Socket connectionSocket = welcomeSocket.accept();
            System.out.println("trafego " + connectionSocket.getTrafficClass() );
            System.out.println("Socket Remoto " + connectionSocket.getRemoteSocketAddress() );

            BufferedReader inFromClient =
                    new BufferedReader(new
                            InputStreamReader(connectionSocket.getInputStream()));

            DataOutputStream  outToClient =
                    new DataOutputStream(connectionSocket.getOutputStream());

            outToClient.write("CONECTADO".getBytes());

            clientSentence = inFromClient.readLine();

            System.out.println("recebido " + clientSentence  );
            if(clientSentence.equals("abcd")){
                outToClient.write("OK".getBytes());
                System.out.println("TO CLIENT: OK");
            } else {
                outToClient.write("NK".getBytes());
                System.out.println("TO CLIENT: NK");
            }

            inFromClient.close();
        }
    }
}