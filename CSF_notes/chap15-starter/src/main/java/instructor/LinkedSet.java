package instructor;

import java.util.ConcurrentModificationException;
import java.util.Iterator;

public class LinkedSet<T> implements Set<T> {
  private final Node<T> head;
  private int version;
  private int numElements;

  /**
   * Construct a LinkedSet.
   */
  public LinkedSet() {
    head = new Node<>(null); // sentinel
    version = 0;
    numElements = 0;
  }

  @Override
  public void insert(T t) {
    if (has(t)) {
      return;
    }
    Node<T> node = new Node<T>(t);
    prepend(node);
    numElements++;
  }

  // Pre: node != null && has(node.data) == false
  // Post: numElements & version are unchanged
  private void prepend(Node<T> node) {
    Node<T> front = head.next;
    node.next = front;
    if (front != null) {
      front.prev = node;
    }
    head.next = node; // new front
    node.prev = head;
  }

  // Linear Search
  // Pre: t != null
  // Post: if found,
  //       the element is moved to the front of the list,
  //       and version++
  private Node<T> find(T t) {
    for (Node<T> n = head.next; n != null; n = n.next) {
      if (n.data.equals(t)) {
        remove(n);
        prepend(n);
        version++;
        return head.next;
      }
    }
    return null;
  }

  @Override
  public void remove(T t) {
    Node<T> target = find(t);
    if (target == null) {
      return;
    }
    remove(target);
    numElements--;
  }

  // Pre: target != null
  // Post: numElements & version are unchanged
  private void remove(Node<T> target) {
    Node<T> prevNode = target.prev;
    Node<T> nextNode = target.next;

    if (prevNode != null) {
      prevNode.next = nextNode;
    }

    if (nextNode != null) {
      nextNode.prev = prevNode;
    }
  }

  @Override
  public boolean has(T t) {
    return find(t) != null;
  }

  @Override
  public int size() {
    return numElements;
  }

  @Override
  public Set<T> union(Set<T> other) {
    Set<T> unionSet = new LinkedSet<>();

    for (T data: this) {
      unionSet.insert(data);
    }

    if (this != other) {
      for (T data: other) {
        unionSet.insert(data);
      }
    }

    return unionSet;
  }

  @Override
  public Set<T> intersect(Set<T> other) {
    Set<T> common = new LinkedSet<>();

    for (T data: this) {
      if (this == other || other.has(data)) {
        common.insert(data);
      }
    }

    return common;
  }

  @Override
  public Set<T> subtract(Set<T> other) {
    Set<T> diff = new LinkedSet<>();

    for (T data: this) {
      if (this != other && !other.has(data)) {
        // we do this != other because otherwise we will get
        // ConcurrentModificationException when has is called!
        diff.insert(data);
      }
    }

    return diff;
  }

  @Override
  public Iterator<T> iterator() {
    return new SetIterator();
  }

  private static class Node<T> {
    T data;
    Node<T> next;
    Node<T> prev;

    Node(T data) {
      this.data = data;
    }
  }

  private class SetIterator implements Iterator<T> {
    private final int version;
    private Node<T> current;

    SetIterator() {
      current = LinkedSet.this.head.next;
      version = LinkedSet.this.version;
    }

    @Override
    public boolean hasNext() {
      checkVersion();
      return current != null;
    }

    @Override
    public T next() {
      checkVersion();
      T t = current.data;
      current = current.next;
      return t;
    }

    private void checkVersion() throws ConcurrentModificationException {
      if (version != LinkedSet.this.version) {
        throw new ConcurrentModificationException();
      }
    }
  }
}
