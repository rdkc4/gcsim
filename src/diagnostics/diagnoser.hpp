#ifndef DIAGNOSER_HPP
#define DIAGNOSER_HPP

#include <ostream>
#include <iostream>

#include "../common/stack/indexed-stack.hpp"
#include "diagnostics.hpp"

/**
 * @class diagnoser
 * @brief handler for diagnostics.
*/
class diagnoser {
private:
    /// stack of diagnostic records.
    indexed_stack<diagnostics> diagnostic_records;

public:
    /**
     * @brief creates the instance of the diagnoser.
    */
    diagnoser() = default;

    /**
     * @brief deletes the instance of the diagnoser.
    */
    ~diagnoser() = default;

    /**
     * @brief creates new diagnostic record.
     * @param diagnostic - new diagnostic record.
    */
    void record(diagnostics diagnostic);

    /**
     * @brief reports all diagnostic records.
     * @param out - output stream.
    */
    void report(std::ostream& out = std::cout) const noexcept;

    /**
     * @brief reports average diagnostic data.
     * @param out - output stream.
    */
    void report_avg(std::ostream& out = std::cout) const noexcept;

};

#endif