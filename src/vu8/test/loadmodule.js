var console = vu8.load('console')
console.println('yo', 'baby', 4.2, null)

var c2 = vu8.load('console')
c2.println('conker 2')

function pando() {
    this.friend = 1
    this.masto = vu8.load('console')
    this.whack = function() {
        this.masto.println('truckman')
    }
}

var c = new pando()
c.whack()

