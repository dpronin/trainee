#ifndef FOO_H_
#define FOO_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct call_table_v1 {
  void (*bar)(void);
};

struct call_table_v2 {
  void (*bar)(int);
};

struct call_table_description {
  int version;
  void const *call_table;
};

__attribute__((visibility("default"))) struct call_table_description get_call_table_description(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FOO_H_ */
