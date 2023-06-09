#ifndef PARSTOOLS_H
#define PARSTOOLS

#include <string>
#include <memory>
#include <google/protobuf/io/coded_stream.h>

template <typename Message>
std::shared_ptr<Message> parseDelimited(const void *data, size_t size, size_t *bytesConsumed = 0)
{
    if (data == nullptr || size == 0)
        return nullptr;

    uint32_t length = 0;
    google::protobuf::io::CodedInputStream stream(static_cast<const uint8_t *>(data), size);
    if(!stream.ReadVarint32(&length))
    {
        return nullptr;
    }

    if (length + stream.CurrentPosition() > size || length < 1)
        return nullptr;

    stream.PopLimit(length + stream.CurrentPosition());

    if (bytesConsumed != nullptr)
    {
        *bytesConsumed += length + stream.CurrentPosition();
    }

    std::shared_ptr<Message> message = std::make_shared<Message>();
    if (message->ParseFromCodedStream(&stream))
        if(stream.ConsumedEntireMessage())
            return message;

    return nullptr;
}

template <typename Message>
class DelimitedMessagesStreamParser
{
public:
    typedef std::shared_ptr<const Message> PointerToConstValue;
    std::list<PointerToConstValue> parse(const std::string &data)
    {
        m_buffer.insert(m_buffer.end(), data.begin(), data.end());

        std::list<typename DelimitedMessagesStreamParser<Message>::PointerToConstValue> messages;
        size_t bytesConsumed = 0;
        size_t prev = 0;
        
        do 
        {
            prev = bytesConsumed;
            std::shared_ptr<Message> message = parseDelimited<Message>(m_buffer.data() + bytesConsumed, m_buffer.size(), &bytesConsumed);
            if (message != nullptr)
                messages.push_back(message);
            else if (bytesConsumed == prev)
            {
                break;
            }
        }
        while(bytesConsumed < m_buffer.size());

        if (bytesConsumed != 0)
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + bytesConsumed);
        
        return messages;
    }

private:
    std::vector<char> m_buffer;
};

#endif // PARSTOOLS_H