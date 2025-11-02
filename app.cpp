#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QLabel>
#include <QProcess>
#include <QFileDialog>
#include <QFileInfo>
#include <cstdlib>
#include <ctime>
#include <string>

int generateRandomInt() {
    static bool seeded = false;
    if (!seeded) {
        std::srand((unsigned)time(0));
        seeded = true;
    }
    return (std::rand() % 10000) + 1;
}

QString writeToPythonFile(const QString &content) {
    int randomInt = generateRandomInt();
    std::string scriptFileName = "TempManimScript_" + std::to_string(randomInt) + ".py";
    QString filename = QString::fromStdString(scriptFileName);
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: Cannot open file for writing!";
        return QString();
    }

    QTextStream out(&file);
    out << content;
    file.close();
    qDebug() << "Successfully wrote to" << filename;
    
    return filename;
}

class AudioWindow : public QWidget {
public:
    AudioWindow(QTextEdit *terminalArea, QWidget *parent = nullptr) 
        : QWidget(parent), terminal(terminalArea) {
        setWindowTitle("Add Background Audio to Video");
        setFixedSize(500, 280);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // Title label
        QLabel *titleLabel = new QLabel("Add Background Audio to Video");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Video selection widget
        QWidget *videoWidget = new QWidget();
        QHBoxLayout *videoLayout = new QHBoxLayout(videoWidget);
        QLabel *videoLabel = new QLabel("Video:");
        videoLabel->setFixedWidth(60);
        videoPathDisplay = new QLabel("No file selected");
        QPushButton *selectVideoBtn = new QPushButton("Browse");
        selectVideoBtn->setFixedWidth(100);
        videoLayout->addWidget(videoLabel);
        videoLayout->addWidget(videoPathDisplay);
        videoLayout->addWidget(selectVideoBtn);
        mainLayout->addWidget(videoWidget);

        // Audio selection widget
        QWidget *audioWidget = new QWidget();
        QHBoxLayout *audioLayout = new QHBoxLayout(audioWidget);
        QLabel *audioLabel = new QLabel("Audio:");
        audioLabel->setFixedWidth(60);
        audioPathDisplay = new QLabel("No file selected");
        QPushButton *selectAudioBtn = new QPushButton("Browse");
        selectAudioBtn->setFixedWidth(100);
        audioLayout->addWidget(audioLabel);
        audioLayout->addWidget(audioPathDisplay);
        audioLayout->addWidget(selectAudioBtn);
        mainLayout->addWidget(audioWidget);

        // Merge button
        QPushButton *mergeBtn = new QPushButton("Merge Audio into Video");
        mainLayout->addWidget(mergeBtn);

        // Close button
        QPushButton *closeBtn = new QPushButton("Close");
        mainLayout->addWidget(closeBtn);

        connect(selectVideoBtn, &QPushButton::clicked, this, [this]() {
            videoPath = QFileDialog::getOpenFileName(this, "Select MP4 Video", "", "Video Files (*.mp4)");
            if (!videoPath.isEmpty()) {
                terminal->append("Selected Video: " + videoPath);
                QFileInfo fileInfo(videoPath);
                videoPathDisplay->setText(fileInfo.fileName());
            }
        });

        connect(selectAudioBtn, &QPushButton::clicked, this, [this]() {
            audioPath = QFileDialog::getOpenFileName(this, "Select MP3 Audio", "", "Audio Files (*.mp3 *.wav)");
            if (!audioPath.isEmpty()) {
                terminal->append("Selected Audio: " + audioPath);
                QFileInfo fileInfo(audioPath);
                audioPathDisplay->setText(fileInfo.fileName());
            }
        });

        connect(mergeBtn, &QPushButton::clicked, this, [this]() {
            if (videoPath.isEmpty() || audioPath.isEmpty()) {
                terminal->append("Error: Please select both video and audio files!\n");
                return;
            }

            int randomInt = generateRandomInt();
            QString outputFile = QString("output_with_music_%1.mp4").arg(randomInt);
            
            QString command = QString("ffmpeg -i \"%1\" -i \"%2\" -c:v copy -c:a aac -shortest \"%3\" -y")
                .arg(videoPath).arg(audioPath).arg(outputFile);

            terminal->append("\nMerging audio with video...");
            terminal->append("Command: " + command + "\n");
            
            QProcess *mergeProcess = new QProcess(this);
            mergeProcess->start("/bin/bash", QStringList() << "-c" << command);
            
            connect(mergeProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    this, [this, mergeProcess, outputFile](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
                    terminal->append("Success! Video saved as: " + outputFile + "\n");
                } else {
                    terminal->append("Error merging video!\n");
                    terminal->append(QString::fromUtf8(mergeProcess->readAllStandardError()) + "\n");
                }
                mergeProcess->deleteLater();
            });
        });

        connect(closeBtn, &QPushButton::clicked, this, [this]() {
            terminal->append("Audio window closed.\n");
            this->close();
        });
    }

private:
    QString videoPath, audioPath;
    QTextEdit *terminal;
    QLabel *videoPathDisplay;
    QLabel *audioPathDisplay;
};





// main func 
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Manim Script Runner");

    // Create UI elements
    QTextEdit *codeArea = new QTextEdit();
    QTextEdit *terminalArea = new QTextEdit();
    terminalArea->setReadOnly(true);

    QPushButton *saveButton = new QPushButton("Save Script");
    QPushButton *runButton = new QPushButton("Run Manim");
    QPushButton *audioButton = new QPushButton("Add Audio");
    QLabel *label = new QLabel("Manim Python Script:");

    // Layout setup
    QHBoxLayout *mainLayout = new QHBoxLayout();
    QVBoxLayout *leftLayout = new QVBoxLayout();
    QVBoxLayout *terminalLayout = new QVBoxLayout();
    
    QLabel *terminalLabel = new QLabel("Terminal Output:");

    leftLayout->addWidget(audioButton);
    leftLayout->addWidget(label);
    leftLayout->addWidget(codeArea);
    leftLayout->addWidget(saveButton);
    leftLayout->addWidget(runButton);
    
    terminalLayout->addWidget(terminalLabel);
    terminalLayout->addWidget(terminalArea);
    
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(terminalLayout);

    window.setLayout(mainLayout);
    window.resize(900, 500);
    window.show();

    // Create AudioWindow instance
    AudioWindow *audioWindow = new AudioWindow(terminalArea, &window);

    // Create process for running python script
    QProcess *process = new QProcess(&window);
    QString currentScriptFile;

    // Connect buttons
    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        QString code = codeArea->toPlainText();
        if (code.isEmpty()) {
            terminalArea->append("Error: No script to save!\n");
            return;
        }
        
        currentScriptFile = writeToPythonFile(code);
        if (!currentScriptFile.isEmpty()) {
            terminalArea->append("Script saved to: " + currentScriptFile + "\n");
        } else {
            terminalArea->append("Failed to save script!\n");
        }
    });

    QObject::connect(runButton, &QPushButton::clicked, [&]() {
        if (currentScriptFile.isEmpty()) {
            terminalArea->append("Error: Please save the script first!\n");
            return;
        }
        
        terminalArea->clear();
        terminalArea->append("Running " + currentScriptFile + "...\n");
        terminalArea->append("Installing dependencies and rendering...\n\n");

        QString command = QString(
            "python3 -m venv env && "
            "source env/bin/activate && "
            "pip install --quiet manim && "
            "manim -pql %1 -r 1080,1920"
        ).arg(currentScriptFile);

        process->start("/bin/bash", QStringList() << "-c" << command);
    });

    QObject::connect(audioButton, &QPushButton::clicked, [audioWindow, terminalArea]() {
        terminalArea->append("Opening audio merger window...\n");
        audioWindow->show();
        audioWindow->raise();
        audioWindow->activateWindow();
    });

    // Show live output from the process
    QObject::connect(process, &QProcess::readyReadStandardOutput, [process, terminalArea]() {
        terminalArea->append(QString::fromUtf8(process->readAllStandardOutput()));
    });

    QObject::connect(process, &QProcess::readyReadStandardError, [process, terminalArea]() {
        QString error = QString::fromUtf8(process->readAllStandardError());
        if (!error.trimmed().isEmpty()) {
            terminalArea->append(error);
        }
    });

    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [terminalArea](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            terminalArea->append("\nProcess completed successfully!\n");
            terminalArea->append("Check the 'media' folder for your rendered video.\n");
        } else {
            terminalArea->append("\nProcess failed with exit code: " + QString::number(exitCode) + "\n");
        }
    });
    
    return app.exec();
}
