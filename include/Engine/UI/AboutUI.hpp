#pragma once

// UI module: stores textual data for the About panel.

namespace ow {

class AboutUI {
public:
    const char* Title() const;
    int LineCount() const;
    const char* Line(int index) const;
};

} // namespace ow
