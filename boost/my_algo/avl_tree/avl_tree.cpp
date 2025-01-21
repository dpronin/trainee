#include <xroost/avl_tree.hpp>

int main(int argc, char const *argv[]) {
  xroost::avl_tree<float> tree;
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
  tree.set_printer(xroost::avl_tree<float>::printer::prefix);
  tree.print();
  tree.set_printer(xroost::avl_tree<float>::printer::infix);
  tree.print();
  tree.set_printer(xroost::avl_tree<float>::printer::postfix);
  tree.print();

  return 0;
}
