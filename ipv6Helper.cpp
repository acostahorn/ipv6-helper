#include "ipv6Helper.h"
#include "ipv6Parser.h"
#include <QFontDatabase>
#include <QFont>

IPv6Helper::IPv6Helper(QWidget *parent)
    : QMainWindow(parent)
{

    centralWidget = new QWidget(this);
    this->setFixedSize(800, 600);

    applicationLabel = new QLabel("IPv6 Helper", this);
    applicationLabel->setObjectName("applicationLabel");

    m_inputArea = new QLineEdit(this);
    m_inputArea->setPlaceholderText("Input your IPv6 shorthand here:");
    m_outputArea = new QTextEdit(this);

    // Sets font for the output area

    int fontId = QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono-Regular.ttf");

    if (fontId != -1)
    {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont monoFont(fontFamily, 10); // Family name and size

        // 2. Apply it directly to your QTextEdit box
        m_outputArea->setFont(monoFont);
    }

    m_cidrArea = new QLineEdit(this);
    m_cidrArea->setPlaceholderText("/64");
    m_cidrArea->setMaximumWidth(80);

    // m_outputArea->setReadOnly(true);
    m_submitButton = new QPushButton(this);
    m_submitButton->setObjectName("submitButton");
    m_submitButton->setText("Submit");

    QHBoxLayout *inputRowLayout = new QHBoxLayout();
    inputRowLayout->addWidget(m_inputArea);
    inputRowLayout->addWidget(m_cidrArea);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(applicationLabel);
    layout->addLayout(inputRowLayout);
    layout->addWidget(m_submitButton);
    layout->addWidget(m_outputArea);
    setCentralWidget(centralWidget);

    connect(m_submitButton, &QPushButton::clicked, this, &IPv6Helper::onSubmitClicked);

    // Optional: Pressing 'Enter' inside the text field triggers the submit button as well
    connect(m_inputArea, &QLineEdit::returnPressed, this, &IPv6Helper::onSubmitClicked);
}

void IPv6Helper::onSubmitClicked()
{

    std::string inputStr = m_inputArea->text().trimmed().toStdString();
    std::string cidrStr = m_cidrArea->text().trimmed().toStdString();

    auto result = IPv6Parser::parseShorthand(inputStr);

    auto cidr = IPv6Parser::checkEnteredCidr(cidrStr);

    bool translatable = false;

    if (!cidr.has_value())
    {
        m_outputArea->setPlainText("Invalid CIDR value");
        return;
    }

    if (result.has_value())
    {

        IPv6Parser::IPv6Address ip(result.value());

        int zeroCount = 0;

        for (int i = 0; i < 5; i++)
        {
            if (ip.blocks[i] == 0)
                ++zeroCount;
        }

        if (zeroCount == 5 && ip.blocks[5] == 0xffff)
            translatable = true;

        std::string canonicalStr = ip.toFullString();

        m_outputArea->clear();

        QString html = "<table>";

        html += QString("<tr>"
                        "<td>Parsed IPv6 address:</td>"
                        "<td><b>%1</b></td>"
                        "</tr>")
                    .arg(canonicalStr.c_str());

        fprintf(stderr, "%d", cidr.value());

        std::array<uint16_t, 8> mask = IPv6Parser::createIPv6Mask(cidr.value());

        IPv6Parser::IPv6Address subnet_mask;

        for (int i = 0; i < 8; i++)
        {
            fprintf(stderr, "%d\n", mask[i]);
            fflush(stderr);
            subnet_mask.blocks[i] = mask[i];
        }

        std::string subnetStr = subnet_mask.toFullString();

        std::array<uint16_t, 8> networkIDArray = IPv6Parser::applyMask(ip.getBlocks(), mask);

        IPv6Parser::IPv6Address network_id;
        network_id.blocks = networkIDArray;

        std::string network_idString = network_id.toFullString();

        IPv6Parser::IPv6Address firstUsableAddress;
        std::array<uint16_t, 8> firstUsableBlocks = networkIDArray;

        if (cidr.value() < 127)
            firstUsableBlocks[7] += 1; // Increment the final 16-bit block
        firstUsableAddress.blocks = firstUsableBlocks;
        std::string firstUsableString = firstUsableAddress.toFullString();

        IPv6Parser::IPv6Address lastAddress;
        std::array<uint16_t, 8> lastAddressBlocks = IPv6Parser::calculateLastAddress(network_id.getBlocks(), mask);
        lastAddress.blocks = lastAddressBlocks;
        std::string lastAddrString = lastAddress.toFullString();

        html += QString("<tr>"
                        "<td>Subnet Mask:</td>"
                        "<td><b>%1</b></td>"
                        "</tr>")
                    .arg(subnetStr.c_str());

        html += QString("<tr>"
                        "<td>Network ID:</td>"
                        "<td><b>%1</b></td>"
                        "</tr>")
                    .arg(network_idString.c_str());
        html += QString("<tr>"
                        "<td>First Usable Address:</td>"
                        "<td><b>%1</b></td>"
                        "</tr>")
                    .arg(firstUsableString.c_str());
        html += QString("<tr>"
                        "<td>Last Address:</td>"
                        "<td><b>%1</b></td>"
                        "</tr>")
                    .arg(lastAddrString.c_str());
        html += QString("<tr>"
                        "<td>Number of Addresses:</td>"
                        "<td><b>2^%1</b></td>"
                        "</tr>")
                    .arg(128 - cidr.value());

        if (translatable)
        {
            std::string ipv4String = "";
            for (int j = 6; j < 8; ++j)
            {
                ipv4String += std::to_string(IPv6Parser::quartetTo2Int(ip.blocks[j])[0]);
                ipv4String += "." + std::to_string(IPv6Parser::quartetTo2Int(ip.blocks[j])[1]);
                if (j == 6) ipv4String +=".";
            }

            html += QString("<tr>"
                        "<td>IPv4 Address:</td>"
                        "<td><b>%1</b></td>"
                        "</tr>")
                    .arg(ipv4String.c_str());
                


        }


        html += "</table>";
        if (translatable && cidr.value()<96) {
            html +="<p><i>Note: Prefix &lt; /96 spans outside the IPv4-mapped address space</i><p>";
        }

        m_outputArea->setHtml(html);
    }
    else
    {
        m_outputArea->setPlainText("Failed to parse IPv6 address.");
    }
}
