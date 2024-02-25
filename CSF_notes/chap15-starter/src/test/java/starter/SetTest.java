package starter;

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
  private Set<String> unit;

  protected abstract Set<String> createUnit();

  @BeforeEach
  void setupTests() {
    unit = this.createUnit();
  }

  @Test
  @DisplayName("Set is empty as soon as it is constructed")
  void newSetEmpty() {
    assertEquals(0, unit.size());
  }

  @Test
  @DisplayName("Test iterator after construction")
  void newSetEmptyIterator() {
    int c = 0;
    for (String s : unit) {
      c++;
    }
    assertEquals(0, c);
  }

  @Test
  @DisplayName("Set is not empty after insertion")
  public void insertNotEmpty() {
    unit.insert("Magda");
    assertEquals(1, unit.size());
  }

  @Test
  @DisplayName("Inserting duplicate values does not change set size")
  public void insertDuplicateSizeConsistent() {
    unit.insert("one");
    unit.insert("one");
    assertEquals(1, unit.size());
  }

  @Test
  @DisplayName("Set contains the element which was inserted")
  public void insertHas() {
    assertFalse(unit.has("one"));
    unit.insert("one");
    assertTrue(unit.has("one"));
  }

  @Test
  @DisplayName("Set does not contain the element which was removed")
  public void insertRemoveHas() {
    unit.insert("one");
    unit.remove("one");
    assertFalse(unit.has("one"));
  }

  @Test
  @DisplayName("Complex set operation: many insert one remove")
  public void manyInsertOneRemove() {
    unit.insert("one");
    unit.insert("two");
    unit.remove("one");
    unit.insert("three");
    assertEquals(2, unit.size());
    assertFalse(unit.has("one"));
    assertTrue(unit.has("two"));
    assertTrue(unit.has("three"));
  }

  @Test
  @DisplayName("size operation works as expected")
  public void insertManySizeConsistent() {
    unit.insert("one");
    unit.insert("two");
    unit.insert("three");
    assertEquals(3, unit.size());
  }

  @Test
  @DisplayName("iterator works as expected")
  public void iteratorWorks() {
    String[] data = {"Peter", "Paul", "Mary", "Beverly"};
    for (String d : data) {
      unit.insert(d);
    }
    for (String s : unit) {
      int count = 0;
      for (String d : data) {
        if (s.equals(d)) {
          count += 1;
        }
      }
      assertEquals(1, count);
    }
  }
}
