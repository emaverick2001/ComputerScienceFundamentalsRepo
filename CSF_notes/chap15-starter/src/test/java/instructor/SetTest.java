package instructor;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

/**
 * Testing implementations of the Set interface.
 */
@SuppressWarnings("All")
public abstract class SetTest {
  private Set<String> set;

  protected abstract Set<String> createSet();

  @BeforeEach
  void setupTests() {
    set = this.createSet();
  }

  @Test
  @DisplayName("Set is empty as soon as it is constructed")
  void newSetEmpty() {
    assertEquals(0, set.size());
  }

  @Test
  @DisplayName("Test iterator on empty set")
  void newSetEmptyIterator() {
    int c = 0;
    for (String s : set) {
      c++;
    }
    assertEquals(0, c);
  }

  @Test
  @DisplayName("Set is not empty after insertion")
  public void insertNotEmpty() {
    set.insert("Magda");
    assertEquals(1, set.size());
  }

  @Test
  @DisplayName("Inserting duplicate values does not change set size")
  public void insertDuplicateSizeConsistent() {
    set.insert("one");
    set.insert("one");
    assertEquals(1, set.size());
  }

  @Test
  @DisplayName("Set contains the element which was inserted")
  public void insertHas() {
    assertFalse(set.has("one"));
    set.insert("one");
    assertTrue(set.has("one"));
  }

  @Test
  @DisplayName("Set does not contain an element which was removed")
  public void insertRemoveHas() {
    set.insert("one");
    set.remove("one");
    assertFalse(set.has("one"));
  }

  @Test
  @DisplayName("Complex set operation: many insert one remove")
  public void manyInsertOneRemove() {
    set.insert("one");
    set.insert("two");
    set.remove("one");
    set.insert("three");
    assertEquals(2, set.size());
    assertFalse(set.has("one"));
    assertTrue(set.has("two"));
    assertTrue(set.has("three"));
  }

  @Test
  @DisplayName("size operation works as expected")
  public void insertManySizeConsistent() {
    set.insert("one");
    set.insert("two");
    set.insert("three");
    assertEquals(3, set.size());
  }

  @Test
  @DisplayName("iterator works as expected")
  public void iteratorWorks() {
    String[] data = {"Peter", "Paul", "Mary", "Beverly"};
    for (String d : data) {
      set.insert(d);
    }
    for (String s : set) {
      int count = 0;
      for (String d : data) {
        if (s.equals(d)) {
          count += 1;
        }
      }
      assertEquals(1, count);
    }
  }

  @Test
  @DisplayName("Union of two sets with non-empty intersection")
  void unionTwoSetWithPartialIntersection() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");
    set2.insert("02");
    set2.insert("03");
    set2.insert("04");

    Set<String> union;
    union = set1.union(set2);
    assertEquals(4, union.size());
    union = set2.union(set1);
    assertEquals(4, union.size());
  }

  @Test
  @DisplayName("Intersection of two sets with non-empty intersection")
  void intersectionTwoSetWithPartialIntersection() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");
    set2.insert("02");
    set2.insert("03");
    set2.insert("04");

    Set<String> common;
    common = set1.intersect(set2);
    assertEquals(1, common.size());
    common = set2.intersect(set1);
    assertEquals(1, common.size());
  }

  @Test
  @DisplayName("Difference of two sets with non-empty intersection")
  void differenceTwoSetWithPartialIntersection() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");
    set2.insert("02");
    set2.insert("03");
    set2.insert("04");

    Set<String> diff;
    diff = set1.subtract(set2);
    assertEquals(1, diff.size());
    diff = set2.subtract(set1);
    assertEquals(2, diff.size());
  }

  @Test
  @DisplayName("Union of two sets with empty intersection")
  void unionTwoSetNoIntersection() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");
    set2.insert("11");
    set2.insert("12");
    set2.insert("13");

    Set<String> union;
    union = set1.union(set2);
    assertEquals(5, union.size());
    union = set2.union(set1);
    assertEquals(5, union.size());
  }

  @Test
  @DisplayName("Intersection of two sets with empty intersection")
  void intersectTwoSetNoIntersection() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");
    set2.insert("11");
    set2.insert("12");
    set2.insert("13");

    Set<String> common;
    common = set1.intersect(set2);
    assertEquals(0, common.size());
    common = set2.intersect(set1);
    assertEquals(0, common.size());
  }

  @Test
  @DisplayName("Difference of two sets with empty intersection")
  void differenceTwoSetNoIntersection() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");
    set2.insert("11");
    set2.insert("12");
    set2.insert("13");

    Set<String> diff;
    diff = set1.subtract(set2);
    assertEquals(2, diff.size());
    diff = set2.subtract(set1);
    assertEquals(3, diff.size());
  }

  @Test
  @DisplayName("Union of two empty sets")
  void unionTwoEmptySets() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();

    Set<String> union;
    union = set1.union(set2);
    assertEquals(0, union.size());
    union = set2.union(set1);
    assertEquals(0, union.size());
  }

  @Test
  @DisplayName("Intersection of two empty sets")
  void intersectionTwoEmptySets() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();

    Set<String> common;
    common = set1.intersect(set2);
    assertEquals(0, common.size());
    common = set2.intersect(set1);
    assertEquals(0, common.size());
  }

  @Test
  @DisplayName("Difference of two empty sets")
  void differenceTwoEmptySets() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();

    Set<String> diff;
    diff = set1.subtract(set2);
    assertEquals(0, diff.size());
    diff = set2.subtract(set1);
    assertEquals(0, diff.size());
  }

  @Test
  @DisplayName("Union of two sets where one is empty")
  void unionTwoSetsOneEmpty() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");

    Set<String> union;
    union = set1.union(set2);
    assertEquals(2, union.size());
    union = set2.union(set1);
    assertEquals(2, union.size());
  }

  @Test
  @DisplayName("Intersection of two sets where one is empty")
  void intersectionTwoSetsOneEmpty() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");

    Set<String> common;
    common = set1.intersect(set2);
    assertEquals(0, common.size());
    common = set2.intersect(set1);
    assertEquals(0, common.size());
  }

  @Test
  @DisplayName("Difference of two sets where one is empty")
  void differenceTwoSetsOneEmpty() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");

    Set<String> diff;
    diff = set1.subtract(set2);
    assertEquals(2, diff.size());
    diff = set2.subtract(set1);
    assertEquals(0, diff.size());
  }

  @Test
  @DisplayName("Union of two sets where one is a subset of other")
  void unionTwoSetsOneSubsetOfOther() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");
    set2.insert("01");
    set2.insert("02");
    set2.insert("03");

    Set<String> union;
    union = set1.union(set2);
    assertEquals(3, union.size());
    union = set2.union(set1);
    assertEquals(3, union.size());
  }

  @Test
  @DisplayName("Intersection of two sets where one is a subset of other")
  void intersectionTwoSetsOneSubsetOfOther() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");
    set2.insert("01");
    set2.insert("02");
    set2.insert("03");

    Set<String> common;
    common = set1.intersect(set2);
    assertEquals(2, common.size());
    assertTrue(common.has("01"));
    assertTrue(common.has("02"));
    assertFalse(common.has("03"));

    common = set2.intersect(set1);
    assertEquals(2, common.size());
    assertTrue(common.has("01"));
    assertTrue(common.has("02"));
    assertFalse(common.has("03"));
  }

  @Test
  @DisplayName("Difference of two sets where one is a subset of other")
  void differenceTwoSetsOneSubsetOfOther() {
    Set<String> set1 = createSet();
    Set<String> set2 = createSet();
    set1.insert("01");
    set1.insert("02");
    set2.insert("01");
    set2.insert("02");
    set2.insert("03");

    Set<String> diff;
    diff = set1.subtract(set2);
    assertEquals(0, diff.size());

    diff = set2.subtract(set1);
    assertEquals(1, diff.size());
    assertFalse(diff.has("01"));
    assertFalse(diff.has("02"));
    assertTrue(diff.has("03"));
  }

  @Test
  @DisplayName("Union of a set with itself")
  void unionSetWithItself() {
    Set<String> set1 = createSet();
    set1.insert("01");
    set1.insert("02");

    Set<String> union;
    union = set1.union(set1);
    assertEquals(2, union.size());
  }

  @Test
  @DisplayName("Intersection of a set with itself")
  void intersectionSetWithItself() {
    Set<String> set1 = createSet();
    set1.insert("01");
    set1.insert("02");

    Set<String> common;
    common = set1.union(set1);
    assertEquals(2, common.size());
  }

  @Test
  @DisplayName("Difference of a set with itself")
  void differenceSetWithItself() {
    Set<String> set1 = createSet();
    set1.insert("01");
    set1.insert("02");

    Set<String> diff;
    diff = set1.subtract(set1);
    assertEquals(0, diff.size());
  }
}
