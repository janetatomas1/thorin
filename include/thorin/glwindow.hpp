
#include "thorin/window.hpp"

namespace thorin {
    class GLWindow: public Window {
    public:
        void init() override;
        void destroy() override;
        void update() override;
    };
}
