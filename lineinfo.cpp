#include "lineinfo.h"

size_t LineInfo::get(size_t offset)
{
    size_t currentOffset = 0;
    size_t line = 0;
    for (size_t i = 0; i < lines.size(); i++)
    {
        currentOffset += lines[i].count;
        if (currentOffset > offset)
        {
            line = lines[i].line;
            break;
        }
    }
    return line;
}

void LineInfo::write(size_t line)
{
    if (lines.size() > 0)
    {
        const auto lastIndex = lines.size() - 1;
        if (lines[lastIndex].line == line)
        {
            lines[lastIndex].count++;
            return;
        }
    }
    LineEntry entry;
    entry.line = line;
    entry.count = 1;
    lines.push_back(entry);
}

void LineInfo::dump()
{
    auto it = lines.begin();
    while (it != lines.end())
    {
        auto entry = *it;
        std::cout << entry.line << ":" << entry.count << " ";
        it++;
    }
    std::cout << std::endl;
}