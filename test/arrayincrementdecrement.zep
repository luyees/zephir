namespace Test;

class ArrayIncrementDecrement
{
    /**
     * @see https://github.com/phalcon/zephir/issues/2020
     */
	public function incrementArray() -> array
	{
	    array a = [0, 0, 1, 1];

	    let a[0] += 1;
	    let a[1] = a[1] + 1;
	    let a[2] += 1;
	    let a[3] = a[3] + 1;

        // Expected [1, 1, 2, 2]
	    return a;
	}

    /**
     * @see https://github.com/phalcon/zephir/issues/2020
     */
	public function decrementArray() -> array
	{
        array a = [1, 1, 2, 2];

	    let a[0] -= 1;
	    let a[1] = a[1] - 1;
	    let a[2] -= 1;
	    let a[3] = a[3] - 1;

        // Expected [0, 0, 1, 1]
	    return a;
	}
}
