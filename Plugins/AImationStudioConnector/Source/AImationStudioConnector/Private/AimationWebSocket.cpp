
#include "AimationWebSocket.h"
#include "AImationStudioConnector.h"
#include "Protocol/ProtocolUtils.h"

#include "ThirdParty/nlohmann_json/json.hpp"

UAimationWebSocket::UAimationWebSocket() : WebSocket(nullptr)
{
}

UAimationWebSocket::~UAimationWebSocket()
{
}

bool UAimationWebSocket::Connect(const FString& InUrl)
{
    if (!WebSocket.IsValid())
    {
        WebSocket = FWebSocketsModule::Get().CreateWebSocket(InUrl);

        if (!WebSocket.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("AimationWebSocket failed to create socket for url"));
            return false;
        }
    }

    m_receiveBuffer.reserve( 64 * 1024);
    WebSocket->SetTextMessageMemoryLimit(64 * 1024);
    WebSocket->OnBinaryMessage().AddRaw(this, &UAimationWebSocket::OnBinaryMessage);
    WebSocket->Connect();
    return true;
}

bool UAimationWebSocket::Disconnect()
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        WebSocket->Close();
        WebSocket = nullptr;
        return true;
    }

    return false;
}

void UAimationWebSocket::OnBinaryMessage(const void* InData, SIZE_T InSize, bool isLastFragment)
{
    // turn incoming data into vector<uint8>
    const uint8* data = reinterpret_cast<const uint8*>(InData);
    std::vector<uint8> dataVector(data, data + InSize);
    m_receiveBuffer.insert(m_receiveBuffer.end(), dataVector.begin(), dataVector.end());

    if (isLastFragment)
    {
        constexpr bool G_ALLOW_JSON_EXCEPTIONS{ true };
        try
        {
            nlohmann::json j = nlohmann::json::from_msgpack(m_receiveBuffer, false, G_ALLOW_JSON_EXCEPTIONS);
            checkf(!j.is_discarded(), TEXT("AimationWebSocket received failed to turn msgpack buffer into json object"));

            if (j.contains("HandlerID"))
            {
                uint32 handlerID = j["HandlerID"];
                if (m_handlers.Contains(handlerID))
                {
                    auto contents = j.dump();
                    FString out{ contents.c_str() };

                    m_handlers[handlerID](out);
                }
            }
        }
        catch (std::exception& e)
        {
            auto exceptionReason = e.what();
            UE_LOG(LogTemp, Error, TEXT("AimationWebSocket received failed to turn msgpack buffer into json object: %s"), UTF8_TO_TCHAR(exceptionReason));
            return;
        }

        // clear the array without resizing the buffer, as resizing can change the layout of message pack and crash
        m_receiveBuffer.clear();
    }
}

