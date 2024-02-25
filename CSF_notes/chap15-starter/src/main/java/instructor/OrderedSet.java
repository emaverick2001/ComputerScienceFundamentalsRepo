package instructor;

/**
 * Ordered set of arbitrary values.
 * Iteration order is based on the values.
 *
 * @param <T> Element type.
 */
public interface OrderedSet<T extends Comparable<T>> extends Iterable<T> {
  /**
   * Insert a value.
   * Set doesn't change if we try to insert an existing value.
   * Post: has(t) == true.
   *
   * @param t Value to insert.
   */
  void insert(T t);

  /**
   * Remove a value.
   * Set doesn't change if we try to remove a non-existent value.
   * Post: has(t) == false.
   *
   * @param t Value to remove.
   */
  void remove(T t);

  /**
   * Test membership of a value.
   *
   * @param t Value to test.
   * @return True if t is in the set, false otherwise.
   */
  boolean has(T t);

  /**
   * Number of values.
   *
   * @return Number of values in the set, always greater equal to 0.
   */
  int size();

  /**
   * Constructing a new set with elements that are in this set or
   * in the other set.
   *
   * @param other set.
   * @return all elements that are in this set or the other set.
   */
  OrderedSet<T> union(OrderedSet<T> other);

  /**
   * Constructing a new set with elements that are in this set and
   * in the other set.
   *
   * @param other set.
   * @return the elements this set and other set have in common.
   */
  OrderedSet<T> intersect(OrderedSet<T> other);

  /**
   * Constructing a new set with elements that are in this set but not
   * in the other set.
   *
   * @param other set.
   * @return the elements in this set but not in the other set.
   */
  OrderedSet<T> subtract(OrderedSet<T> other);
}
