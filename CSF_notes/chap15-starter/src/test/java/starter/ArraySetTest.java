package starter;

/**
 * Instantiate the specific Set to test.
 */
public class ArraySetTest extends SetTest {
  @Override
  protected Set<String> createUnit() {
    return new ArraySet<>();
  }
}
