var console = vu8.load('console')

console.log('yo', 'baby', 4.2, null)

var c2 = vu8.load('console')
c2.log('conker 2')

function pando() {
    this.friend = 1
    this.masto = vu8.load('console')
    this.whack = function() {
        this.masto.log('truckman')
    }
}

var c = new pando()
c.whack()

