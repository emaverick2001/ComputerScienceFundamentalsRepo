package starter;

/**
 * Instantiate the LinkedSet to test.
 */
public class LinkedSetTest extends SetTest {
  @Override
  protected Set<String> createUnit() {
    return new LinkedSet<>();
  }
}
