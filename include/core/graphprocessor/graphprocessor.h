#ifndef GRAPHPROCESSOR_H
#define GRAPHPROCESSOR_H

// Qt includes
#include <QFile>
#include <QProcess>
// App includes
#include <graphprocessor/node.h>


namespace dcis::GraphProcessor {



class GraphProcessor
{
public:
    GraphProcessor();
    void setCommGraph(commonGraph* graph);
    size_t  getDronesCount();
    Node* getDroneNode(size_t id);
    commonGraph* getCommGraph();
    void initGraph();
    void initGraphDirs();
    void clearCycles();
    void generateMap();
    void generateGraph();
    void startExploration();
    void startAttack();
    void setImgSize(size_t imgW, size_t imgH);
    static void sendFileToDrone(QString serverIP, int port, QFile file)
    {
        qDebug() << "serverIPp: " << serverIP << " portp: " << port;

        // Replace these values with your own
        QString username = "user";
        //QString serverIP = "178.160.253.146";
        //QString hostname = "proxy50.rt3.io";
        //int port = 37608;  // Replace with your desired SSH port

        //QString remoteCommand = "mkdir jujul";

        QString privateKeyPath = "C:\\Users\\Atash\\.ssh\\id_rsa";
        //QString localFilePath = "C:\\Users\\Atash\\Desktop\\build-DCIS-Desktop_Qt_6_5_0_MSVC2019_64bit-Debug\\CMakeCache.txt";
        //QString localFilePath = "/c/Users/Atash/Desktop/build-DCIS-Desktop_Qt_6_5_0_MSVC2019_64bit-Debug/CMakeCache.txt";
        QString localFilePath = file.fileName();
        localFilePath.replace("/","\\");
        QString remoteFilePath = "/home/user";

        // Specify the SCP command and its arguments
        QString scpCommand = "scp";
        QStringList scpArguments;

        // Add the SSH key, port, source, and destination as arguments
        scpArguments /*<< "-P" << QString::number(port)*/ << "-i" << privateKeyPath
                                                          << localFilePath << username + "@" + serverIP + ":" + remoteFilePath;

        // Create a QProcess instance and start the SCP process
        QProcess scpProcess;
        scpProcess.start(scpCommand, scpArguments);
        scpProcess.waitForFinished();
        qDebug() << scpArguments;
        // Check if the process started successfully
        if (scpProcess.state() == QProcess::Running) {
            qDebug() << "SCP transfer in progress...";
        } else {
            qDebug() << "Failed to start the SCP process. Error:" << scpProcess.errorString();
        }

        // Specify the SSH command and its arguments
        QString sshCommand = "ssh";
        QStringList sshArguments;

        // Add the username, hostname, and port as arguments
        sshArguments /*<< "-p" << QString::number(port)*/ << username + "@" + serverIP;

        QString remoteCommand = "python3 /home/user/Desktop/DCIS/droneside/droneprocessor.py -data /home/user/drone.data > run_dron.log";
        sshArguments << remoteCommand;

        // Create a QProcess instance and start the SSH process
        QProcess sshProcess;
        sshProcess.start(sshCommand, sshArguments);
        sshProcess.waitForStarted();

        if (sshProcess.state() == QProcess::Running) {
            qDebug() << "SSH connection established. Running remote command: " << remoteCommand;

            // Wait for the SSH process to finish
            sshProcess.waitForFinished();

            // Read and print the command output (stdout)
            QByteArray output = sshProcess.readAllStandardOutput();
            qDebug() << "Command output:\n" << output;

            // Read and print any errors (stderr)
            QByteArray error = sshProcess.readAllStandardError();
            qDebug() << "Command error (if any):\n" << error;
        } else {
            qDebug() << "Failed to start the SSH process. Error:" << sshProcess.errorString();
        }
        /*
    if (!file.open(QIODevice::ReadOnly)) {
        // Handle file not found or other errors
        dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: File not found");
        return;
    }

    QTcpSocket socket;
    socket.connectToHost(serverIP, port);

    if (socket.waitForConnected()) {
        QByteArray fileData = file.readAll();
        socket.write(fileData);
        socket.waitForBytesWritten();
        socket.disconnectFromHost();
        file.close();
        dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "File sent successfully");
    } else {
        // Handle connection error
        dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: Failed to connect to the drone");
    }*/

        //QString sshPath = "/home/agit";
        //QString username = "pi";
        //QString password = "raspberry";

        //QString remoteFilePath = "/path/on/remote/server/destination_file";
        //QString localFilePath = "/path/on/local/machine/source_file";
        //QString remoteCommand = "drone.py";

        // SSH connection parameters
        //QStringList sshArgs;
        //sshArgs << "-p" << QString::number(port) << username + "@" + serverIP;

        // File copy parameters
        //QStringList scpArgs;
        //scpArgs << "-P" << QString::number(port) << localFilePath << username + "@" + ipAddress + ":" + remoteFilePath;

        // Remote command execution parameters
        //QStringList sshExecArgs;
        //sshExecArgs << "-p" << QString::number(port) << username + "@" + serverIP << remoteCommand;

        // Start the SSH process for the connection
        //QProcess sshProcess;
        //sshProcess.setProgram(sshPath);
        //sshProcess.setArguments(sshArgs);
        //sshProcess.start();
        //sshProcess.waitForFinished();

        // Copy the file to the remote server using SCP
        //QProcess scpProcess;
        //scpProcess.setProgram("scp");
        //scpProcess.setArguments(scpArgs);
        //scpProcess.start();
        //scpProcess.waitForFinished();

        // Execute the remote command
        //QProcess sshExecProcess;
        //sshExecProcess.setProgram(sshPath);
        //sshExecProcess.setArguments(sshExecArgs);
        //sshExecProcess.start();
        //sshExecProcess.waitForFinished();

        // Check if there were any errors in the processes
        //if (sshProcess.exitCode() != 0 || /*scpProcess.exitCode() != 0 ||*/ sshExecProcess.exitCode() != 0) {
        //    qDebug() << "Error occurred during SSH, SCP, or remote command execution.";
        //    qDebug() << "SSH Process error: " << sshProcess.errorString();
        //qDebug() << "SCP Process error: " << scpProcess.errorString();
        //    qDebug() << "Remote Command Process error: " << sshExecProcess.errorString();
        //} else {
        //    qDebug() << "SSH, SCP, and remote command executed successfully.";
        //}
    }

private:

    commonGraph* commGraph_ = nullptr;
    NodeVectorType vecDronesStartNodes_;
    NodeVectorType vecAttackerNodes_;
    std::vector<QString> vecExplorationFileNames_;
    std::vector<QString> vecAttackFileNames_;
    NodeListType lstNodes_;
    Node* startNode_ = nullptr;
    CommonNodeToNodeMapType commNodeToNodeMap_;
    size_t neighboursCount_ = 0;
    size_t imgW_ = 0;
    size_t imgH_ = 0;
};

#endif // GRAPHPROCESSOR_H

} // end namespace dcis::GraphProcesso
