package instructor;

/**
 * Instantiate the LinkedSet to test.
 */
public class LinkedSetTest extends SetTest {
  @Override
  protected Set<String> createSet() {
    return new LinkedSet<>();
  }
}
