package instructor;

/**
 * Instantiate the specific Set to test.
 */
public class ArraySetTest extends SetTest {
  @Override
  protected Set<String> createSet() {
    return new ArraySet<>();
  }
}
