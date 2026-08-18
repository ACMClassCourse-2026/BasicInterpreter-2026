#pragma once

namespace basic {

class Environment;

class ScopeGuard {
public:
    explicit ScopeGuard(Environment& environment);
    ~ScopeGuard();
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

private:
    Environment& environment_;  // Borrows the guarded environment.
};

}  // namespace basic
