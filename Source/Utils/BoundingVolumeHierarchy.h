#pragma once

#include "Math/Math.h"

using namespace Silent::Math;

// References:
// https://github.com/erincatto/box2d/blob/28adacf82377d4113f2ed00586141463244b9d10/src/dynamic_tree.c
// https://www.gdcvault.com/play/1025909/Math-for-Game-Developers-Dynamic

// NOTE: `_leafIdMap` is a hash map for convenience. If performance suffers with too many `Move` and `Remove` calls, a method with faster access can be implemented.
// However, it requires maintaining an odd index variable outside the BVH instance, so a hash map is preferred for the benefit of cleaner code.

namespace Silent::Utils
{
    enum class BvhBuildStrategy
    {
        Fast,     // O(n): Fast build, okay quality. Top-down approach with median split.
        Balanced, // O(n * m): Efficient build, good quality. Top-down approach with constrained surface area heuristic.
        Accurate  // O(n²): Slow build, optimal quality. Top-down approach with exhaustive surface area heuristic.
    };

    /** @brief Dynamic bounding volume hierarchy using AABBs. */
    class BoundingVolumeHierarchy
    {
    private:
        struct Node
        {
            int                    ObjectId = NO_VALUE; // NOTE: Only stored by leaf.
            AxisAlignedBoundingBox Aabb     = AxisAlignedBoundingBox();

            int Height       = 0;
            int ParentId     = NO_VALUE;
            int LeftChildId  = NO_VALUE;
            int RightChildId = NO_VALUE;

            bool IsLeaf() const;
        };

        // Fields

        std::vector<Node>            _nodes       = {};
        std::stack<int>              _freeNodeIds = {};
        std::unordered_map<int, int> _leafIdMap   = {}; // Key = object ID, value = leaf ID.
        int                          _rootId      = NO_VALUE;

    public:
        // Constructors

        BoundingVolumeHierarchy() = default;
        BoundingVolumeHierarchy(const std::vector<int>& objectIds, const std::vector<AxisAlignedBoundingBox>& aabbs, BvhBuildStrategy strategy = BvhBuildStrategy::Balanced);

        // Getters

        uint GetSize() const;

        std::vector<int> GetBoundedObjectIds() const;
        std::vector<int> GetBoundedObjectIds(const Ray& ray, float dist) const;
        std::vector<int> GetBoundedObjectIds(const BoundingSphere& sphere) const;
        std::vector<int> GetBoundedObjectIds(const AxisAlignedBoundingBox& aabb) const;
        std::vector<int> GetBoundedObjectIds(const OrientedBoundingBox& obb) const;

        // Inquirers

        bool IsEmpty() const;

        // Utilities

        void Insert(int objectId, const AxisAlignedBoundingBox& aabb, float boundary = 0.0f);
        void Move(int objectId, const AxisAlignedBoundingBox& aabb, float boundary = 0.0f);
        void Remove(int objectId);

    private:
        // Collision helpers

        std::vector<int> GetBoundedObjectIds(const std::function<bool(const Node& node)>& testCollRoutine) const;

        // Dynamic helpers

        int GetNewNodeId();
        int GetBestSiblingLeafId(int leafId) const;

        void InsertLeaf(int leafId);
        void RemoveLeaf(int leafId);
        void RefitNode(int nodeId);
        void RemoveNode(int nodeId);
        int  BalanceNode(int nodeId);

        // Static helpers

        void Build(const std::vector<int>& objectIds, const std::vector<AxisAlignedBoundingBox>& aabbs, BvhBuildStrategy strategy);
        int  Build(const std::vector<int>& objectIds, const std::vector<AxisAlignedBoundingBox>& aabbs, int start, int end, BvhBuildStrategy strategy);

        // Debug helpers

        void Validate() const;
        void Validate(int nodeId) const;
    };
}
