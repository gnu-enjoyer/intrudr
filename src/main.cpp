#include <cstddef>
#include <cstdio>
#include <dlfcn.h>
#include <functional>

template <typename FnSignature> FnSignature& HookFn(const char* func_name) {
  static FnSignature real_func = nullptr;
  if (!real_func) {
    real_func = reinterpret_cast<FnSignature>(dlsym(RTLD_NEXT, func_name));
  }
  return real_func;
}

template <typename Hook, typename Real>
auto CreateHookedFunction(Hook&& hook, Real&& real_func) {
  return [&](auto&&... args) -> decltype(auto) {
    hook(args...);
    return real_func(std::forward<decltype(args)>(args)...);
  };
}

template <typename FnSignature, typename... HookArgs>
auto HookAndInvoke(const char* func_name, HookArgs&&... hook_args) {
  return CreateHookedFunction(std::forward<HookArgs>(hook_args)...,
                              HookFn<FnSignature>(func_name));
}

extern "C" void* malloc(size_t size) noexcept {
  return HookAndInvoke<void* (*)(size_t)>("malloc", [&](size_t size) {
    fprintf(stderr, "malloc(%zu)\n", size);
  })(size);
}

extern "C" void free(void* ptr) noexcept {
  HookAndInvoke<void (*)(void*)>(
      "free", [&](void* ptr) { fprintf(stderr, "free(%p)\n", ptr); })(ptr);
}
