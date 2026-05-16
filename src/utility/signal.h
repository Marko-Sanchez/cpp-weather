#ifndef SIGNAL_H
#define SIGNAL_H

#include <functional>
#include <memory>
#include <utility>
#include <variant>

namespace utility
{
/*
 * Broadcaster notifying registered clients. When connected returns a Token (shared_ptr)
 * when the object holding this token is destroyed, the lambda is removed from the array.
 *
 * monostate is simply an empty struct, but it more expressivly states the intention that we
 * only care about the shared_ptr lifetime and not the type it holds.
 */
class Signal
{
    private:

        std::vector<std::pair<std::weak_ptr<std::monostate>, std::function<void()> >> m_slots;

    public:

        using Token = std::shared_ptr<std::monostate>;

        [[nodiscard]] Token Connect(std::function<void()> slot)
        {
            auto token = std::make_shared<std::monostate>();
            m_slots.emplace_back(token, std::move(slot));

            return token;
        }

        void Broadcast()
        {
            std::erase_if(m_slots, [](const auto& slot) {return slot.first.expired();});
            for (auto& [weak, func]: m_slots)
            {
                func();
            }
        }
};
}// namespace utility
#endif
