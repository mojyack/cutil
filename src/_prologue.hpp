#ifndef CUTIL_PROLOGUE
// prologue

#define CUTIL_PROLOGUE

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif
inline namespace CUTIL_MODULE_NAME {

#else
// epilogue
} // namespace CUTIL_MODULE_NAME
#ifdef CUTIL_NS
}
#endif

#undef CUTIL_MODULE_NAME
#undef CUTIL_PROLOGUE

#endif
