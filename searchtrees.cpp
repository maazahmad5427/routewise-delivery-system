#include "RouteWise.h"

TreeNode::TreeNode(Parcel* item)
{
    parcel = item;
    left = NULL;
    right = NULL;
    height = 1;
}

ParcelBST::ParcelBST()
{
    root = NULL;
}

ParcelBST::~ParcelBST()
{
    destroy(root);
}

TreeNode* ParcelBST::insertNode(TreeNode* node, Parcel* parcel)
{
    if (node == NULL)
        return new TreeNode(parcel);

    if (parcel->trackingId < node->parcel->trackingId)
        node->left = insertNode(node->left, parcel);
    else if (parcel->trackingId > node->parcel->trackingId)
        node->right = insertNode(node->right, parcel);

    return node;
}

void ParcelBST::insert(Parcel* parcel)
{
    if (parcel != NULL)
        root = insertNode(root, parcel);
}

Parcel* ParcelBST::searchNode(TreeNode* node, int trackingId) const
{
    if (node == NULL)
        return NULL;

    if (trackingId == node->parcel->trackingId)
        return node->parcel;
    if (trackingId < node->parcel->trackingId)
        return searchNode(node->left, trackingId);
    return searchNode(node->right, trackingId);
}

Parcel* ParcelBST::search(int trackingId) const
{
    return searchNode(root, trackingId);
}

void ParcelBST::inOrder(TreeNode* node) const
{
    if (node == NULL)
        return;

    inOrder(node->left);
    cout << node->parcel->trackingId << "  ";
    inOrder(node->right);
}

void ParcelBST::displayInOrder() const
{
    if (root == NULL)
    {
        cout << "BST is empty.\n";
        return;
    }

    cout << "BST parcel IDs in ascending order: ";
    inOrder(root);
    cout << endl;
}

void ParcelBST::destroy(TreeNode* node)
{
    if (node == NULL)
        return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

int ParcelBST::getHeight(TreeNode* node) const
{
    if (node == NULL)
        return 0;

    int leftHeight = getHeight(node->left);
    int rightHeight = getHeight(node->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int ParcelBST::height() const
{
    return getHeight(root);
}

ParcelAVL::ParcelAVL()
{
    root = NULL;
}

ParcelAVL::~ParcelAVL()
{
    destroy(root);
}

int ParcelAVL::nodeHeight(TreeNode* node) const
{
    if (node == NULL)
        return 0;
    return node->height;
}

int ParcelAVL::balanceFactor(TreeNode* node) const
{
    if (node == NULL)
        return 0;
    return nodeHeight(node->left) - nodeHeight(node->right);
}

TreeNode* ParcelAVL::rotateRight(TreeNode* top)
{
    TreeNode* newTop = top->left;
    TreeNode* movedSubtree = newTop->right;

    newTop->right = top;
    top->left = movedSubtree;

    int leftHeight = nodeHeight(top->left);
    int rightHeight = nodeHeight(top->right);
    top->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);

    leftHeight = nodeHeight(newTop->left);
    rightHeight = nodeHeight(newTop->right);
    newTop->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);

    return newTop;
}

TreeNode* ParcelAVL::rotateLeft(TreeNode* top)
{
    TreeNode* newTop = top->right;
    TreeNode* movedSubtree = newTop->left;

    newTop->left = top;
    top->right = movedSubtree;

    int leftHeight = nodeHeight(top->left);
    int rightHeight = nodeHeight(top->right);
    top->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);

    leftHeight = nodeHeight(newTop->left);
    rightHeight = nodeHeight(newTop->right);
    newTop->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);

    return newTop;
}

TreeNode* ParcelAVL::insertNode(TreeNode* node, Parcel* parcel)
{
    if (node == NULL)
        return new TreeNode(parcel);

    if (parcel->trackingId < node->parcel->trackingId)
        node->left = insertNode(node->left, parcel);
    else if (parcel->trackingId > node->parcel->trackingId)
        node->right = insertNode(node->right, parcel);
    else
        return node;

    int leftHeight = nodeHeight(node->left);
    int rightHeight = nodeHeight(node->right);
    node->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);

    int balance = balanceFactor(node);

    // Left-left case
    if (balance > 1 && parcel->trackingId < node->left->parcel->trackingId)
        return rotateRight(node);

    // Right-right case
    if (balance < -1 && parcel->trackingId > node->right->parcel->trackingId)
        return rotateLeft(node);

    // Left-right case
    if (balance > 1 && parcel->trackingId > node->left->parcel->trackingId)
    {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right-left case
    if (balance < -1 && parcel->trackingId < node->right->parcel->trackingId)
    {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

void ParcelAVL::insert(Parcel* parcel)
{
    if (parcel != NULL)
        root = insertNode(root, parcel);
}

Parcel* ParcelAVL::searchNode(TreeNode* node, int trackingId) const
{
    if (node == NULL)
        return NULL;

    if (trackingId == node->parcel->trackingId)
        return node->parcel;
    if (trackingId < node->parcel->trackingId)
        return searchNode(node->left, trackingId);
    return searchNode(node->right, trackingId);
}

Parcel* ParcelAVL::search(int trackingId) const
{
    return searchNode(root, trackingId);
}

void ParcelAVL::inOrder(TreeNode* node) const
{
    if (node == NULL)
        return;

    inOrder(node->left);
    cout << node->parcel->trackingId << "  ";
    inOrder(node->right);
}

void ParcelAVL::displayInOrder() const
{
    if (root == NULL)
    {
        cout << "AVL tree is empty.\n";
        return;
    }

    cout << "AVL parcel IDs in ascending order: ";
    inOrder(root);
    cout << endl;
}

void ParcelAVL::destroy(TreeNode* node)
{
    if (node == NULL)
        return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

int ParcelAVL::height() const
{
    return nodeHeight(root);
}
