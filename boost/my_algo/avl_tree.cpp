#include <iostream>
#include <utility>
#include <memory>
#include <algorithm>
#include <stack>

template<typename Type>
class DefaultAllocator {
public:
	DefaultAllocator() noexcept = default;
	~DefaultAllocator() noexcept = default;
	template<typename...Types>
	static Type* allocate(Types&&...args) { return new Type(std::forward<Types>(args)...); }
	static void deallocate(Type *&ptr)    { delete ptr; ptr = nullptr; }
};

template<typename ValueType, typename Allocator = DefaultAllocator<ValueType>>
class AvlTree {
private:
	using value_type = ValueType;
	using allocator = Allocator;
	using this_type = AvlTree<value_type, allocator>;

	class AvlNode {
	friend class AvlTree<value_type, allocator>;
	public:
		AvlNode(value_type *pValue) noexcept : pValue_(pValue) {}
		~AvlNode() noexcept = default;

		value_type& operator*() { return *pValue_; }

		size_t fixHeight()
		{
			auto const leftHeight = pLeft_ ? pLeft_->fixHeight() : 0;
			auto const rightHeight = pRight_ ? pRight_->fixHeight() : 0;
			return height_ = std::max(leftHeight, rightHeight) + 1;
		}

		ssize_t getBFactor() const noexcept
		{
			return (pLeft_ ? pLeft_->height_ : 0) - (pRight_ ? pRight_->height_ : 0);
		}

		value_type& getValue() const noexcept { return *pValue_; }
		size_t getHeight() const noexcept { return height_; }

	private:
		value_type *pValue_;
		size_t height_ = 1;
		std::shared_ptr<AvlNode> pLeft_, pRight_;
	};

	class IPrinterStrategy {
	public:
		IPrinterStrategy() noexcept = default;
		virtual ~IPrinterStrategy() noexcept = default;

		virtual void operator()(std::shared_ptr<AvlNode> const &pTreeRoot) const noexcept = 0;
	};

	class PreorderPrinterStrategy : public IPrinterStrategy {
	public:
		PreorderPrinterStrategy() noexcept = default;
		~PreorderPrinterStrategy() noexcept override = default;

	private:
		void operator()(std::shared_ptr<AvlNode> const &pTreeRoot) const noexcept override {
			if (!pTreeRoot) return;

			std::stack<std::shared_ptr<AvlNode>> nodes;
			nodes.push(pTreeRoot);
			while (!nodes.empty()) {
				auto pNode = nodes.top();
				nodes.pop();
				std::cout << *(*pNode) << " ";
				if (pNode->pRight_) nodes.push(pNode->pRight_);
				if (pNode->pLeft_) nodes.push(pNode->pLeft_);
			}
			std::cout << std::endl;
		}
	};

	class InorderPrinterStrategy : public IPrinterStrategy {
	public:
		InorderPrinterStrategy() noexcept = default;
		~InorderPrinterStrategy() noexcept override = default;

	private:
		void operator()(std::shared_ptr<AvlNode> const &pTreeRoot) const noexcept override {
			if (!pTreeRoot) return;

			std::stack<std::shared_ptr<AvlNode>> nodes;
			auto pNode = pTreeRoot;
			while (pNode || !nodes.empty()) {
				while (pNode)
				{
					nodes.push(pNode);
					pNode = pNode->pLeft_;
				}

				pNode = nodes.top();
				nodes.pop();
				std::cout << *(*pNode) << " ";

				pNode = pNode->pRight_;
			}
			std::cout << std::endl;
		}
	};

	class PostorderPrinterStrategy : public IPrinterStrategy {
	public:
		PostorderPrinterStrategy() noexcept = default;
		~PostorderPrinterStrategy() noexcept override = default;

	private:
		void operator()(std::shared_ptr<AvlNode> const &pTreeRoot) const noexcept override {
				if (!pTreeRoot) return;

				std::stack<std::shared_ptr<AvlNode>> first, second;
				first.push(pTreeRoot);
				while (!first.empty())
				{
					auto pNode = first.top();
					first.pop();
					second.push(pNode);
					if (pNode->pLeft_) first.push(pNode->pLeft_);
					if (pNode->pRight_) first.push(pNode->pRight_);
				}

				while (!second.empty())
				{
					auto pNode = second.top();
					second.pop();
					std::cout << *(*pNode) << " ";
				}
		}
	};

public:
	AvlTree() noexcept = default;
	~AvlTree() noexcept = default;

	enum class printer {
		  prefix
		, infix
		, postfix
	};

	std::pair<std::shared_ptr<AvlNode>, bool> insert(value_type const &value) noexcept {
		return insert_(pRoot_, value);
	}

	void printHeight() const noexcept {
		auto pLeft = pRoot_ ? pRoot_->pLeft_ : 0;
		auto pRight = pRoot_ ? pRoot_->pRight_ : 0;
		std::cout
			<< "leftHeight == " << (pLeft ? pLeft->height_ : 0)
			<< "; rightHeight == " << (pRight ? pRight->height_ : 0)
			<< std::endl;
	}

	void print() {
		if (pPrinter_) (*pPrinter_)(pRoot_);
		std::cout << std::endl;
	}

	void setPrinter(printer id) {
		switch (id) {
			case printer::prefix:
				pPrinter_ = std::make_unique<PreorderPrinterStrategy>();
				break;
			case printer::infix:
				pPrinter_ = std::make_unique<InorderPrinterStrategy>();
				break;
			case printer::postfix:
				pPrinter_ = std::make_unique<PostorderPrinterStrategy>();
				break;
			default:
				break;
		}
	}

private:
	void rightRotate_(std::shared_ptr<AvlNode> &pNode) {
		auto qNode = pNode->pLeft_;
		pNode->pLeft_ = qNode->pRight_;
		qNode->pRight_ = pNode;
		pNode = qNode;
	}

	void leftRotate_(std::shared_ptr<AvlNode> &pNode) {
		auto qNode = pNode->pRight_;
		pNode->pRight_ = qNode->pLeft_;
		qNode->pLeft_ = pNode;
		pNode = qNode;
	}

	void balance_(std::shared_ptr<AvlNode> &pNode) {
		pNode->fixHeight();
		auto const bfactor = pNode->getBFactor();
		// right subtree is heavier than the left by 2, required to rotate big to the right
		if (-2 == bfactor)
		{
			// left subtree of the subtree is heavier than the right, required to rotate small to the right
			if (pNode->pRight_->getBFactor() > 0) rightRotate_(pNode->pRight_);
			leftRotate_(pNode);
		}
		// left subtree is heavier than the right by 2, required to rotate big to the left
		else if (2 == bfactor)
		{
			// right subtree of the subtree is heavier than the left, required to rotate small to the left
			if (pNode->pLeft_->getBFactor() < 0) leftRotate_(pNode->pLeft_);
			rightRotate_(pNode);
		}
	}

	std::pair<std::shared_ptr<AvlNode>, bool> insert_(std::shared_ptr<AvlNode> &pNode, value_type const &value) {
		std::pair<std::shared_ptr<AvlNode>, bool> result;
		if (!pNode) {
			auto *ptrValue = allocator::allocate(value);
			auto *ptrNode = DefaultAllocator<AvlNode>::allocate(ptrValue);
			pNode = std::shared_ptr<AvlNode>(ptrNode, [](AvlNode *ptrNode){
				allocator::deallocate(ptrNode->pValue_);
				DefaultAllocator<AvlNode>::deallocate(ptrNode);
			});
			result = std::make_pair(pNode, true);
		}
		else if (value == *(*pNode)) {
			result = std::make_pair(pNode, false);
		}
		else if (value < *(*pNode)) {
			result = insert_(pNode->pLeft_, value);
			if (result.second) balance_(pNode->pLeft_);
			balance_(pNode);
		}
		else {
			result = insert_(pNode->pRight_, value);
			if (result.second) balance_(pNode->pRight_);
			balance_(pNode);
		}

		return result;
	}

private:
	std::shared_ptr<AvlNode> pRoot_;
	std::unique_ptr<IPrinterStrategy> pPrinter_;
};

int main(int argc, char const *argv[]) {
	AvlTree<float> tree;
	tree.insert(4.0);
	tree.insert(7.1);
	tree.insert(8.3);
	tree.insert(2.4);
	tree.insert(5.2);
	tree.insert(9.8);
	tree.insert(10.9);
	tree.insert(11.1);
	tree.insert(1.2);
	tree.insert(3.55);
	tree.insert(6.09);
	tree.setPrinter(AvlTree<float>::printer::prefix);
	tree.print();
	tree.setPrinter(AvlTree<float>::printer::infix);
	tree.print();
	tree.setPrinter(AvlTree<float>::printer::postfix);
	tree.print();

	return 0;
}
