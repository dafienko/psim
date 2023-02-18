const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");

const CANVAS_WIDTH = canvas.width;
const CANVAS_HEIGHT = canvas.height;

const GRID_WIDTH = 240;
const GRID_HEIGHT = 100;

const CELL_WIDTH = Math.min(CANVAS_WIDTH / GRID_WIDTH, CANVAS_HEIGHT / GRID_HEIGHT);
const CELL_HEIGHT = CELL_WIDTH;

const DRAW_TEXT = false;
const DRAW_VELOCITY = DRAW_TEXT;
let RENDER_FLUID_AS = "density";
const RUN_LOOP = true;
const ADVECT = true;

const DENSITY_DECAY = .95;
const INIT_VEL = 39;
const STREAM_WIDTH = 3;
const CIRCLE_RADIUS = 4;

const NUM_ITERS = 20;
const SOLVE_DT = .49;
const ADVECT_DT = .02;

function createGrid(w=GRID_WIDTH, h=GRID_HEIGHT, init=0) {
	let grid = [];
	
	for (let y = 0; y < h; y++) {
		let row = []
		for (let x = 0; x < w; x++) {
			val = init;
			if (typeof init == "function") {
				val = init(x, y);
			}

			row.push(val);
		}
		grid.push(row);
	}

	return grid;
}

let uGrid = createGrid(GRID_WIDTH + 1, GRID_HEIGHT + 1), vGrid = createGrid(GRID_WIDTH + 1, GRID_HEIGHT + 1);
let sGrid = createGrid();
let dGrid = createGrid();
let densGrid = createGrid();

function dist(a, b, x, y) {
	return Math.sqrt(Math.pow(y - b, 2) + Math.pow(x - a, 2));
}

let oGrid = createGrid(GRID_WIDTH, GRID_HEIGHT, init = (x, y) => {
	if (dist(x, y, Math.floor(GRID_WIDTH / 3), Math.floor(GRID_HEIGHT / 2)) < CIRCLE_RADIUS) {
		return 0;
	}

	if (y == 0 || x == 0 || y == GRID_HEIGHT - 1 || x == GRID_WIDTH - 1) {
		return 0;
	} else {
		return 1;
	}
});

for (let i = -STREAM_WIDTH; i <= STREAM_WIDTH; i++) {
	uGrid[Math.floor(GRID_HEIGHT / 2 + i)][1] = INIT_VEL;
	densGrid[Math.floor(GRID_HEIGHT / 2 + i)][0] = 1;
}

function get(grid, x, y) {
	if (x < 0 || y < 0 || y >= grid.length || x >= grid[0].length) {
		return 0;
	}

	return grid[y][x];
}

function calcDS() {
	for (let y = 0; y < GRID_HEIGHT; y++) {
		for (let x = 0; x < GRID_WIDTH; x++) {
			let left = get(uGrid, x, y);
			let bottom = get(vGrid, x, y);
			let right = get(uGrid, x+1, y);
			let top = get(vGrid, x, y+1);

			if (oGrid[y][x] == 0) {
				dGrid[y][x] = 0;
				sGrid[y][x] = 0;
			} else {
				dGrid[y][x] = (left + bottom - (right + top));
				sGrid[y][x] = get(oGrid, x-1, y) + get(oGrid, x+1, y) + get(oGrid, x, y-1) + get(oGrid, x, y+1);
			}
		}
	}
}

function solveGrid(dt) {
	for (let y = 0; y <= GRID_HEIGHT; y++) {
		for (let x = 0; x <= GRID_WIDTH; x++) {
			let d = get(dGrid, x, y);
			let s = get(sGrid, x, y);
			let o = get(oGrid, x, y)
			
			if (s == 0) {
				continue;
			}

			let ld = get(dGrid, x-1, y);
			let ls = get(sGrid, x-1, y);
			let lo = get(oGrid, x-1, y);

			let bd = get(dGrid, x, y-1);
			let bs = get(sGrid, x, y-1);
			let bo = get(oGrid, x, y-1);
			
			if (s > 0) {
				uGrid[y][x] += -d * (lo / s) * dt;
				vGrid[y][x] += -d * (bo / s) * dt;
			}

			if (ls > 0) {
				uGrid[y][x] += ld * (o / ls) * dt;
			}
			if (bs > 0) {
				vGrid[y][x] += bd * (o / bs) * dt;
			}
				
		}
	}
}

function linear(a, b, alpha) {
	return a * (1 - alpha) + b * alpha;
}

function bilinear(a, b, c, d, x, y, lerp) {
	let u = lerp(a, b, x);
	let v = lerp(c, d, x);
	return lerp(v, u, y);
}

function sampleU(x, y) {
	let blx = Math.floor(x);
	let bly = Math.floor(y - .5);
	let ax = x - blx;
	let ay = y - bly - .5;

	return bilinear(
		get(uGrid, blx, bly+1),
		get(uGrid, blx+1, bly+1),
		get(uGrid, blx, bly),
		get(uGrid, blx+1, bly),
		ax, ay, linear
	);
}

function sampleV(x, y) {
	let blx = Math.floor(x - .5);
	let bly = Math.floor(y);
	let ax = x - blx - .5;
	let ay = y - bly;

	return bilinear(
		get(vGrid, blx, bly+1),
		get(vGrid, blx+1, bly+1),
		get(vGrid, blx, bly),
		get(vGrid, blx+1, bly),
		ax, ay, linear
	);
}

function sampleDens(x, y) {
	let blx = Math.floor(x - .5);
	let bly = Math.floor(y - .5);
	let ax = x - blx - .5;
	let ay = y - bly - .5;

	return bilinear(
		get(densGrid, blx, bly+1),
		get(densGrid, blx+1, bly+1),
		get(densGrid, blx, bly),
		get(densGrid, blx+1, bly),
		ax, ay, linear
	);
}

function cloneGrid(grid) {
	let copy = [];

	for (let i = 0; i < grid.length; i++) {
		let row = [];
		for (let j = 0; j < grid[i].length; j++) {
			row.push(grid[i][j]);
		}
		copy.push(row);
	}

	return copy;
}

function advect(dt) {
	let newU = cloneGrid(uGrid);
	let newV = cloneGrid(vGrid);
	let newDens = cloneGrid(densGrid);

	for (let y = 0; y <= GRID_HEIGHT; y++) {
		for (let x = 0; x <= GRID_WIDTH; x++) {
			u = [uGrid[y][x], sampleV(x, y+.5)];
			if (get(oGrid, Math.round(x - u[0] * dt), Math.round(y+.5 - u[1] * dt)) != 0) { 
				newU[y][x] = sampleU(x - u[0] * dt, y+.5 - u[1] * dt)
			}

			v = [sampleU(x+.5, y), vGrid[y][x]];
			if (get(oGrid, Math.round(x+.5 - v[0] * dt), Math.round(y - v[1] * dt)) != 0) {
				newV[y][x] = sampleV(x+.5 - v[0] * dt, y - v[1] * dt)
			}
		}
	}

	let decay = Math.pow(DENSITY_DECAY, dt);
	for (let y = 0; y < GRID_HEIGHT; y++) {
		for (let x = 0; x < GRID_WIDTH; x++) {
			if (oGrid[y][x] == 0) {
				continue;
			}

			densGrid[y][x] *= decay;
		}
	}

	for (let y = 0; y < GRID_HEIGHT; y++) {
		for (let x = 0; x < GRID_WIDTH; x++) {
			if (oGrid[y][x] == 0) {
				continue;
			}

			vel = [sampleU(x+.5, y+.5), sampleV(x+.5, y+.5)];
			newDens[y][x] = sampleDens(x+.5 - vel[0] * dt, y+.5 - vel[1] * dt);
		}
	}

	densGrid = newDens;
	uGrid = newU;
	vGrid = newV;
}

function getRGBStr(r, g, b) {
	r = Math.max(0, Math.min(Math.round(r), 255))
	g = Math.max(0, Math.min(Math.round(g), 255))
	b = Math.max(0, Math.min(Math.round(b), 255))
	return "rgb(" + r + "," + g + "," + b + ")";
}

function setFillColor(r, g, b) {
	ctx.fillStyle = getRGBStr(r, g, b)
}

function setStrokeColor(r, g, b) {
	ctx.strokeStyle = getRGBStr(r, g, b)
}

function drawGrid() {
	ctx.clearRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
	
	function gridToScreen(x, y) {
		return [x * CELL_WIDTH, -y * CELL_HEIGHT + CANVAS_HEIGHT];
	}

	function drawCell(x, y, r, g, b) {
		setFillColor(r, g, b);
		let pos = gridToScreen(x, y);
		ctx.fillRect(pos[0], pos[1], CELL_WIDTH, -CELL_HEIGHT);
	}

	// draw 'pressure'
	ctx.font = "20px serif";
	for (let y = 0; y < GRID_HEIGHT; y++) {
		for (let x = 0; x < GRID_WIDTH; x++) {
			let d = dGrid[y][x];
			if (oGrid[y][x] == 0) {
				drawCell(x, y, 255, 255, 255);
			} else {
				if (RENDER_FLUID_AS == "velocity") {
					let u = (get(uGrid, x, y) + get(uGrid, x+1, y)) / 2;
					let v = (get(vGrid, x, y) + get(vGrid, x, y+1)) / 2;
					drawCell(x, y, Math.abs(u / 10) * 255, Math.abs(v / 10) * 255, 0);
				} else if (RENDER_FLUID_AS == "pressure") {
					let range = 2;
					drawCell(x, y, -d * 255 / range, -d * 255 / range, d * 255 / range);
				} else if (RENDER_FLUID_AS == "density") {
					let dens = densGrid[y][x] * 255;
					drawCell(x, y, dens * .1, dens * .4, dens);
				}
			}
			
			if (DRAW_TEXT) {
				let pos = gridToScreen(x, y);
				pos[0] += CELL_WIDTH/4;
				pos[1] -= CELL_WIDTH/2;

				setFillColor(255, 0, 255);
				ctx.fillText((Math.round(d * 100) / 100).toString(), pos[0], pos[1])

				setFillColor(255, 0, 255);
				ctx.fillText((Math.round(sGrid[y][x])).toString(), pos[0], pos[1] + 22)
			}
		}
	}

	function drawArrow(x, y, u, v) {
		ctx.moveTo(x, y);
		ctx.lineTo(x+u/10, y+v/10);
	}

	// draw velocity
	if (DRAW_VELOCITY) {
		setStrokeColor(255, 0, 0);
		ctx.beginPath()
		for (let y = 0; y <= GRID_HEIGHT; y++) {
			for (let x = 0; x <= GRID_WIDTH; x++) {
				let bl = gridToScreen(x, y);
				
				let u = uGrid[y][x];
				drawArrow(bl[0], bl[1] - CELL_HEIGHT / 2, u * CELL_WIDTH, 0);
			}
		}
		ctx.stroke()

		setStrokeColor(0, 255, 0);
		ctx.beginPath()
		for (let y = 0; y <= GRID_HEIGHT; y++) {
			for (let x = 0; x <= GRID_WIDTH; x++) {
				let bl = gridToScreen(x, y);
			
				let v = vGrid[y][x];
				drawArrow(bl[0] + CELL_WIDTH / 2, bl[1], 0, -v * CELL_HEIGHT);
			}
		}
		ctx.stroke();
	}
}

function update() {
	RENDER_FLUID_AS = document.getElementById("renderAs").value;

	for (let i = 0; i < NUM_ITERS; i++) {
		calcDS();
		solveGrid(SOLVE_DT);
	}

	if (ADVECT) {
		advect(ADVECT_DT);
	}
}

function render() {
	drawGrid();
}

render();

function loop(_) {
	update();
	render();

	window.requestAnimationFrame(loop)
}

if (RUN_LOOP) {
	window.requestAnimationFrame(loop);
}

addEventListener('keypress', (event) => {
	if (event.key == 's') {
		update();
		render();
	}
});