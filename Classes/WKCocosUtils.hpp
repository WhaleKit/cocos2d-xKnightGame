#ifndef  _WK_COCOS_UTILS_HPP_
#define  _WK_COCOS_UTILS_HPP_
#include <type_traits>
#include <any.hpp>

#include "cocos2d.h"

using nonstd::any;
using nonstd::any_cast;
using nonstd::make_any;
using nonstd::swap;
using nonstd::bad_any_cast;

//file for cocos2d-x specific utilities
namespace {

template< class Base, class Derived >
constexpr bool is_base_of_v = std::is_base_of<Base, Derived>::value;


//that's so obvious, I wonder why isn't is in the cocos itself
//or it is, but not in the documentation?
/** @class resSharedPtr
 * @brief does Ref::release() and Ref::retain() for you, using magic of RAII
 *
 */
template <
         typename T,
         typename TIsRef = std::enable_if_t<is_base_of_v<cocos2d::Ref, T>,
                                            std::true_type
                                            >
          >
class resSharedPtr
{
private:
    inline static void safeRelease(T* ptr)
    {
        if (ptr != nullptr){
            ptr->release();
        }
    }
    inline static void safeRetain(T* ptr)
    {
        if (ptr != nullptr){
            ptr->retain();
        }
    }
    T* m_ptr = nullptr;
public:
    resSharedPtr(T* ptr = nullptr) //default and from T* constructor
    {
        static_assert(TIsRef{}, "Nani? Enable_if shouldn't have let this happen!");
        //also I heard somewhere type of enable_if should be used somewhere is the template class
        m_ptr = ptr;
        safeRetain(m_ptr);
    }
    resSharedPtr(resSharedPtr const& from) //assigment constructor
    {
        this->m_ptr = from.m_ptr;
        safeRetain(m_ptr);
    }
    resSharedPtr(resSharedPtr&& from) //move constructor
    {
        this->m_ptr = from.m_ptr;
        from.m_ptr = nullptr;
    }
    //my favorite thing about destructors is that you can't forget to call them
    ~resSharedPtr()
    {
        safeRelease(m_ptr);
    }
    /**
     * @brief releases old resource, captures new
     * @param ptr  pointer to a new resource
     */
    void reset(T* ptr = nullptr)
    {
        safeRelease(m_ptr);
        m_ptr = ptr;
        safeRetain(m_ptr);
    }
    resSharedPtr& operator = (resSharedPtr const& rhs) //same
    {
        this->reset(rhs.m_ptr);
        return *this;
    }
    T* get()
    {
        return m_ptr;
    }
    /**
     * @brief cget
     * @return constant pointer to stored resource
     */
    T const* cget() const
    {
        return m_ptr;
    }
    T const* get() const
    {
        return m_ptr;
    }
    /**
     * @brief check if resSharedPtr contains cocos2d-x resource
     * @return true if resSharedPtr contains resource
     */
    bool hasResource () const //nullptr check
    {
        return m_ptr != nullptr;
    }
    /**
     * @brief operator bool
     * @see 'hasResource()'
     */
    operator bool () const
    {
        return hasResource();
    }
    /**
     * @brief inverted hasResource()
     * @return true if resSharedPtr does not contain resource
     * @see 'hasResource()'
     */
    bool operator!() const
    {
        return !hasResource();
    }
    T const& operator*() const
    {
        return *m_ptr;
    }
    T const& operator ->() const
    {
        return *m_ptr;
    }
    T& operator*()
    {
        return *m_ptr;
    }
    T& operator ->()
    {
        return *m_ptr;
    }
};
//won't be needed in c++ 17 thx to deduction guides
template <typename T> inline
resSharedPtr<T> makeResHaredPtr(T* ccResource)
{
    return resSharedPtr<T>(ccResource);
}

/**
 * @brief scales (keeping proportions) node so that it fits inside a box
 *
 * @param node  node to scale
 * @param box   size inside which node will be scaled
 */
void MakeFitInsideASize(cocos2d::Node* node, cocos2d::Size box)
{
    if (!node) {
        return;
    }
    cocos2d::Size nodeSize  = node->getBoundingBox().size;
    float xFactor = box.width / nodeSize.width ;
    float yFactor = box.height/ nodeSize.height;
    float factor = std::min(xFactor, yFactor);
    float oldScaleX = node->getScaleX();
    float oldScaleY = node->getScaleY();
    node->setScale(oldScaleX * factor, oldScaleY * factor);
}
void PutAnchorInCenter(cocos2d::Node* node)
{
//    cocos2d::Vec2 center = node->getBoundingBox().size / 2;
    node->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
}

//not really a cocos-specific util,
//source https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}
/*
template <typename T>
struct sharedObject : public cocos2d::Ref
{
    T m;
};*/
/**
 * @brief std::any-like thing m in struct derived from cocos2d::Ref
 * any "any" can be used, but here it's this one: https://github.com/martinmoene/any-lite
 */
struct sharedAny : public cocos2d::Ref
{
    sharedAny() = default;
    sharedAny(sharedAny const&) = default;
    sharedAny(sharedAny &&) = default;
    sharedAny(any const& arg)
        :m(arg)
    {    }
    sharedAny(any&& arg)
        :m(std::move(arg))
    {    }
    ~sharedAny() = default;
    any m;
};

float mmToPixels(float mm)
{
    float pixelsPerMm =  cocos2d::Device::getDPI() / 25.4;
    return mm*pixelsPerMm;
    //fuckin' ell
}

cocos2d::Vec2 convertBetweenNodeSpaces (cocos2d::Node* fromNode,
                                        cocos2d::Node* toNode,
                                        cocos2d::Vec2 vec)
{
    auto worldSpVec = fromNode->convertToWorldSpace(vec);
    return toNode->convertToNodeSpace(worldSpVec);
}
cocos2d::Rect convertBetweenNodeSpaces (cocos2d::Node* fromNode,
                                        cocos2d::Node* toNode,
                                        cocos2d::Rect rect)
{
    auto origPoint = rect.origin;
    auto origPlusSize = rect.origin + rect.size;
    origPoint   = convertBetweenNodeSpaces(fromNode, toNode, origPoint   );
    origPlusSize= convertBetweenNodeSpaces(fromNode, toNode, origPlusSize);
    return cocos2d::Rect(origPoint, cocos2d::Size{origPlusSize - origPoint});
}
cocos2d::Size convertBetweenNodeSpaces (cocos2d::Node* fromNode,
                                        cocos2d::Node* toNode,
                                        cocos2d::Size size) //now I see why Size and Vec2 is different types
{
    cocos2d::Rect fromSpaceRect{cocos2d::Vec2::ZERO, size};
    cocos2d::Rect toSpaceRect = convertBetweenNodeSpaces(fromNode, toNode, fromSpaceRect);
    return toSpaceRect.size;
}



} //namespace

//hash functions for cocos2d types go there
namespace std {
template <> struct hash<cocos2d::Vec2>
{
    size_t operator()(const cocos2d::Vec2& x) const
    {
        size_t s = 0;
        hash_combine(s, x.x);
        hash_combine(s, x.y);
        return s;
    }
};

template <> struct hash<cocos2d::Vec3>
{
    size_t operator()(const cocos2d::Vec3& x) const
    {
        size_t s = 0;
        hash_combine(s, x.x);
        hash_combine(s, x.y);
        hash_combine(s, x.z);
        return s;
    }
};
template <> struct hash<cocos2d::Mat4>
{
    size_t operator()(const cocos2d::Mat4& x) const
    {
        size_t s = 0;
        for (int i=0; i<16; ++i) {
            hash_combine(s, x.m[i]);
        }
        return s;
    }
};

}//namespace std



#endif // _WK_COCOS_UTILS_HPP_
