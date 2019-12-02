<?php

/*
 * This file is part of the Zephir.
 *
 * (c) Zephir Team <team@zephir-lang.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace Extension;

use PHPUnit\Framework\TestCase;
use Test\ArrayIncrementDecrement;

class ArrayIncrementDecrementTest extends TestCase
{
    /**
     * @test
     *
     * @see https://github.com/phalcon/zephir/issues/2020
     */
    public function shouldIncrementArrayValues()
    {
        $test = new ArrayIncrementDecrement();
        $this->assertEquals([1, 1, 2, 2], $test->incrementArray());
    }

    /**
     * @test
     *
     * @see https://github.com/phalcon/zephir/issues/2020
     */
    public function shouldDecrementArrayValues()
    {
        $test = new ArrayIncrementDecrement();
        $this->assertEquals([0, 0, 1, 1], $test->decrementArray());
    }
}
