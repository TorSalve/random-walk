#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/states.hpp"
#include "ultraleap/haptics/control_point.hpp"
#include "ultraleap/haptics/library.hpp"
#include "ultraleap/haptics/device.hpp"
#include "ultraleap/haptics/version.hpp"

namespace Ultraleap
{
namespace Haptics
{

template <typename StateType>
class Solver
{
public:
    virtual bool solve(const ControlPoint* points, size_t size, StateType& result) = 0;
};

class SolverFactory
{
public:
    virtual Solver<FocusPointState>* createFocusPointSolver(const Device& device) = 0;
    virtual Solver<TransducersState>* createTransducerSolver(const Device& device) = 0;

    virtual void destroyFocusPointSolver(Solver<FocusPointState>* solver) = 0;
    virtual void destroyTransducerSolver(Solver<TransducersState>* solver) = 0;
};

class ULH_RESEARCH_API_CLASS ResearchLibrary : public Library
{
public:
    ResearchLibrary(AppVersionInfo app_info)
        : Library(app_info)
    {
        init();
    }

    ResearchLibrary()
        : Library()
    {
        init();
    }

    ULH_RESEARCH_API ResearchLibrary(const ResearchLibrary& other);
    ULH_RESEARCH_API ResearchLibrary& operator=(const ResearchLibrary& other);
    ULH_RESEARCH_API ResearchLibrary(ResearchLibrary&& other) noexcept;
    ULH_RESEARCH_API ResearchLibrary& operator=(ResearchLibrary&& other) noexcept;

    ULH_RESEARCH_API virtual result<void> connect() override;
    ULH_RESEARCH_API virtual result<void> connect(const char* connect_path) override;

    ULH_RESEARCH_API SolverFactory* getSolverFactory() const;
    ULH_RESEARCH_API bool setSolverFactory(SolverFactory* new_factory);

private:
    ULH_RESEARCH_API void init();
};

} // namespace Haptics

} // namespace Ultraleap
