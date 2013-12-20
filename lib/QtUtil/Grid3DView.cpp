#include <QtUtil/Grid3DView.hpp>

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

#include <stdexcept>

#include <cassert>



namespace
{
	const unsigned int DEFAULT_SIZE = 200;

	const unsigned int DEFAULT_WIDTH = 5;

	const unsigned char BG_O = 190;
	const unsigned char BG_E = 193;
	const unsigned char BG_D = 10;

	const QColor BG_ODD        (BG_O, BG_O, BG_O);
	const QColor BG_EVEN       (BG_E, BG_E, BG_E);
	const QColor BG_LIGHT_ODD  (BG_O + BG_D, BG_O + BG_D, BG_O + BG_D);
	const QColor BG_LIGHT_EVEN (BG_E + BG_D, BG_E + BG_D, BG_E + BG_D);
	const QColor BG_DARK_ODD   (BG_O - BG_D, BG_O - BG_D, BG_O - BG_D);
	const QColor BG_DARK_EVEN  (BG_E - BG_D, BG_E - BG_D, BG_E - BG_D);

	const QColor AXIS_X (Qt :: red);
	const QColor AXIS_Y (Qt :: green);
	const QColor AXIS_Z (Qt :: blue);

	const int AXIS_MARK_OFFSET = 10;
	const int AXIS_MARK_LENGTH = 15;

	const QPen CURSOR_ABOVE (Qt :: white, 0, Qt :: DashLine);
	const QPen CURSOR_BELOW (Qt :: black, 0, Qt :: DashLine);
	const QPen CURSOR_LEVEL (Qt :: black, 0);
}



QtUtil :: Grid3DView :: SharedState :: ~ SharedState ()
{
}



QtUtil :: Grid3DView :: DragInfo :: DragInfo ()
: dragging (false)
, di (0)
, dk (0)
{
}



QtUtil :: Grid3DView :: Position :: Position ()
: x (0)
, y (0)
, z (0)
{
}



QtUtil :: Grid3DView :: Position :: Position (int _x, int _y, int _z)
: x (_x)
, y (_y)
, z (_z)
{
}



bool
QtUtil :: Grid3DView :: Position :: operator == (const Position & p)
const
{
	return x == p .x && y == p .y && z == p .z;
}



bool
QtUtil :: Grid3DView :: Position :: operator != (const Position & p)
const
{
	return ! (*this == p);
}



QtUtil :: Grid3DView :: Position &
QtUtil :: Grid3DView :: Position :: operator += (const Position & p)
{
	x += p .x;
	y += p .y;
	z += p .z;

	return *this;
}



QtUtil :: Grid3DView :: Position
QtUtil :: Grid3DView :: Position :: operator + (const Position & p)
const
{
	Position pos (*this);
	pos += p;
	return pos;
}



QtUtil :: Grid3DView :: Grid3DView (QWidget * parent)
: QWidget (parent)
, m_plane (Plane :: XY)
, m_width (DEFAULT_WIDTH)
{
	setMinimumSize (DEFAULT_SIZE, DEFAULT_SIZE);

	setSizePolicy (QSizePolicy :: Expanding, QSizePolicy :: Expanding);
}



QtUtil :: Grid3DView :: ~ Grid3DView ()
{
	if (m_shared_state)
		m_shared_state -> m_views .remove (this);
}



void
QtUtil :: Grid3DView :: set_position (int x, int y, int z)
{
	if (m_position != Position (x, y, z))
	{
		m_position = {x, y, z};

		emit position_changed (x, y, z);

		update ();
	}
}



void
QtUtil :: Grid3DView :: set_width (int w)
{
	if (w >= 0 && w != int (m_width))
	{
		m_width = w;

		update ();
	}
}



void
QtUtil :: Grid3DView :: set_plane (Plane p)
{
	if (m_plane != p)
	{
		m_plane = p;

		update ();
	}
}



void
QtUtil :: Grid3DView :: paintEvent (QPaintEvent *)
{
	QPainter p (this);

	paint (p, width (), height ());
}



void
QtUtil :: Grid3DView :: paint (QPainter & p, int w, int h)
const
{
	const int SIZE = 2 * m_width + 1;

	// Background

	p .fillRect (0, 0, w, h, Qt :: gray);

	p .setWindow (QRect (0, SIZE, SIZE, -SIZE));

	for (int i = -m_width; i <= int (m_width); ++i)
	{
		for (int k = -m_width; k <= int (m_width); ++k)
		{
			int I = get_i (m_position) + i;
			int K = get_k (m_position) + k;

			const bool dark  = (0 == I % 10)  || (0 == K % 10);
			const bool light = (0 == I % 100) || (0 == K % 100);

			const auto & ODD  = dark ? BG_DARK_ODD  : BG_ODD;
			const auto & EVEN = dark ? BG_DARK_EVEN : BG_EVEN;

			p .fillRect (
				i + m_width,
				k + m_width,
				1,
				1,
				(I^K)&1
					? (light ? BG_LIGHT_ODD  : ODD)
					: (light ? BG_LIGHT_EVEN : EVEN));
		}
	}

	for (int i = -m_width; i <= int (m_width); ++i)
	{
		for (int k = -m_width; k <= int (m_width); ++k)
		{
			p .save ();

			p .translate (i + m_width, k + m_width);

			draw (p, m_position + to_global (i, k));

			p .restore ();
		}
	}

	draw_cursor (p, w, h);

	draw_axes (p, w, h);
}



void
QtUtil :: Grid3DView :: draw_cursor (QPainter & p, int, int)
const
{
	int c_i = get_i (m_cursor) - get_i (m_position);
	int c_k = get_k (m_cursor) - get_k (m_position);

	if (get_m (m_cursor) > get_m (m_position))
		p .setPen (CURSOR_ABOVE);
	else if (get_m (m_cursor) < get_m (m_position))
		p .setPen (CURSOR_BELOW);
	else
		p .setPen (CURSOR_LEVEL);

	auto arrow = [&] (int x, int y, int dir)
	{
		p .save ();

		p .translate (m_width + x + 0.5, m_width + y + 0.5);
		
		p .rotate (dir * 360 / 8);

		p .scale (0.15, 0.15);

		p .drawLine (0,  0, 3, 0);
		p .drawLine (2,  1, 3, 0);
		p .drawLine (2, -1, 3, 0);

		p .restore ();
	};

	const int W = m_width;

	if (c_i < -W)
	{
		if (c_k < -W)
			arrow (-W, -W, 5);
		else if (c_k > W)
			arrow (-W, W, 3);
		else
			arrow (-W, c_k, 4);
	}
	else if (c_i > W)
	{
		if (c_k < -W)
			arrow (W, -W, 7);
		else if (c_k > W)
			arrow (W, W, 1);
		else
			arrow (W, c_k, 0);
	}
	else if (c_k < -W)
	{
		assert (-W <= c_i && c_i <= W);

		arrow (c_i, -W, 6);
	}
	else if (c_k > W)
	{
		assert (-W <= c_i && c_i <= W);

		arrow (c_i, W, 2);
	}
	else
	{
		assert (-W <= c_i && c_i <= W);
		assert (-W <= c_k && c_k <= W);

		p .drawRect (c_i + W, c_k + W, 1, 1);

		const int di = m_drag_info .di;
		const int dk = m_drag_info .dk;

		if (m_drag_info .dragging && (di || dk))
		{
			int angle = 0;

			if (dk == 0)
			{
				angle = (di > 0) ? 0 : 4;
			}
			else
			{
				if (di > 0)
					angle = 1;
				else if (0 == di)
					angle = 2;
				else
					angle = 3;

				if (dk < 0)
					angle = -angle;
			}

			arrow (c_i + di, c_k + dk, angle);
		}
	}
}



void
QtUtil :: Grid3DView :: draw_axes (QPainter & p, int, int h)
const
{
	p .save ();

	p .resetTransform ();

	// Horizontal (i)

	switch (m_plane)
	{

	case Plane :: XY:
		p .setPen (QPen (AXIS_X, 0));
		break;

	case Plane :: XZ:
		p .setPen (QPen (AXIS_X, 0));
		break;

	case Plane :: YZ:
		p .setPen (QPen (AXIS_Z, 0));
		break;
	};

	p .drawLine (
		AXIS_MARK_OFFSET, h - AXIS_MARK_OFFSET,
		AXIS_MARK_OFFSET + AXIS_MARK_LENGTH, h - AXIS_MARK_OFFSET);

	// Vertical (k)

	switch (m_plane)
	{

	case Plane :: XY:
		p .setPen (QPen (AXIS_Y, 0));
		break;

	case Plane :: XZ:
		p .setPen (QPen (AXIS_Z, 0));
		break;

	case Plane :: YZ:
		p .setPen (QPen (AXIS_Y, 0));
		break;
	};

	p .drawLine (
		AXIS_MARK_OFFSET, h - AXIS_MARK_OFFSET,
		AXIS_MARK_OFFSET, h - (AXIS_MARK_OFFSET + AXIS_MARK_LENGTH));

	p .restore ();
}



void
QtUtil :: Grid3DView :: contextMenuEvent (QContextMenuEvent * e)
{
	QMenu m;

	QMenu set_plane (tr ("Plane"));
	{
		set_plane .addAction (tr ("XY (Front)"), this, SLOT (set_xy()));
		set_plane .addAction (tr ("XZ (Top)"),   this, SLOT (set_xz()));
		set_plane .addAction (tr ("YZ (Side)"),  this, SLOT (set_yz()));
	}
	
	m .addMenu (& set_plane);

	m .exec (mapToGlobal (e -> pos ()));
}



void
QtUtil :: Grid3DView :: set_xy ()
{
	set_plane (Plane :: XY);
}



void
QtUtil :: Grid3DView :: set_xz ()
{
	set_plane (Plane :: XZ);
}



void
QtUtil :: Grid3DView :: set_yz ()
{
	set_plane (Plane :: YZ);
}



void
QtUtil :: Grid3DView :: keyPressEvent (QKeyEvent * e)
{
	const bool ctrl = e -> modifiers () & Qt :: ControlModifier;

	const int MOVE = ctrl
		? 1 + m_width / 2
		: 1;

	auto quick_drag = [&] ()
	{
		auto delta = to_global (m_drag_info .di, m_drag_info .dk);

		drag (delta);

		auto p = m_position + delta;

		set_position (p .x, p .y, p .z);

		m_drag_info = DragInfo ();

		m_drag_info .dragging = true;
	};

	switch (e -> key ())
	{

	case Qt :: Key_Shift:
		m_drag_info .dragging = true;
		QWidget :: keyPressEvent (e);
		return;

	case Qt :: Key_Equal:
		set_width (m_width + 1);
		break;

	case Qt :: Key_Minus:
		set_width (m_width - 1);
		break;

	case Qt :: Key_Up:
		if (m_drag_info .dragging)
		{
			m_drag_info .dk = 1;

			if (ctrl)
				quick_drag ();

			update ();
		}
		else
			move_k (MOVE);
		break;

	case Qt :: Key_Down:
		if (m_drag_info .dragging)
		{
			m_drag_info .dk = -1;

			if (ctrl)
				quick_drag ();

			update ();
		}
		else
			move_k (-MOVE);
		break;

	case Qt :: Key_Right:
		if (m_drag_info .dragging)
		{
			m_drag_info .di = 1;

			if (ctrl)
				quick_drag ();

			update ();
		}
		else
			move_i (MOVE);
		break;

	case Qt :: Key_Left:
		if (m_drag_info .dragging)
		{
			m_drag_info .di = -1;

			if (ctrl)
				quick_drag ();

			update ();
		}
		else
			move_i (-MOVE);
		break;

	case Qt :: Key_PageUp:
		move_m (MOVE);
		break;

	case Qt :: Key_PageDown:
		move_m (-MOVE);
		break;

	default:
		QWidget :: keyPressEvent (e);
		return;
	};

	e -> accept ();
}



void
QtUtil :: Grid3DView :: keyReleaseEvent (QKeyEvent * e)
{
	switch (e -> key ())
	{

	case Qt :: Key_Shift:
		
		if (0 == m_drag_info .di && 0 == m_drag_info .dk)
		{
			m_drag_info = DragInfo ();

			QWidget :: keyReleaseEvent (e);

			update ();

			return;
		}
		
		drag (to_global (m_drag_info .di, m_drag_info .dk));

		m_drag_info = DragInfo ();
		
		break;

	case Qt :: Key_Up:
	case Qt :: Key_Down:
		
		if (false == m_drag_info .dragging)
		{
			QWidget :: keyReleaseEvent (e);

			return;
		}
		
		m_drag_info .dk = 0;
		
		update ();

		break;

	case Qt :: Key_Left:
	case Qt :: Key_Right:
		if (false == m_drag_info .dragging)
		{
			QWidget :: keyReleaseEvent (e);

			return;
		}

		m_drag_info .di = 0;
		
		update ();

		break;
	};

	e -> accept ();
}



void
QtUtil :: Grid3DView :: drag (const Position &)
{
}



void
QtUtil :: Grid3DView :: enterEvent (QEvent *)
{
	setFocus (Qt :: MouseFocusReason);
}



void
QtUtil :: Grid3DView :: leaveEvent (QEvent *)
{
	if (parentWidget ())
		parentWidget () -> setFocus (Qt :: MouseFocusReason);
}



int
QtUtil :: Grid3DView :: get_i (const Position & p)
const
{
	if (Plane :: YZ == m_plane)
		return p .z;
	else
		return p .x;
}



int
QtUtil :: Grid3DView :: get_k (const Position & p)
const
{
	if (Plane :: XZ == m_plane)
		return p .z;
	else
		return p .y;
}



int
QtUtil :: Grid3DView :: get_m (const Position & p)
const
{
	switch (m_plane)
	{

	case Plane :: XY:
		return p .z;

	case Plane :: XZ:
		return p .y;

	case Plane :: YZ:
		return p .x;

	default:
		throw std :: logic_error (__PRETTY_FUNCTION__);
	};
}



void
QtUtil :: Grid3DView :: move_i (int delta)
{
	Position p (m_position);

	if (Plane :: YZ == m_plane)
		p += {0, 0, delta};
	else
		p += {delta, 0, 0};

	set_position (p .x, p .y, p .z);
}



void
QtUtil :: Grid3DView :: move_k (int delta)
{
	Position p (m_position);

	if (Plane :: XZ == m_plane)
		p += {0, 0, delta};
	else
		p += {0, delta, 0};

	set_position (p .x, p .y, p .z);
}



void
QtUtil :: Grid3DView :: move_m (int delta)
{
	Position p (m_position);

	if (Plane :: XY == m_plane)
		p .z += delta;
	else if (Plane :: XZ == m_plane)
		p .y += delta;
	else
		p .x += delta;

	set_position (p .x, p .y, p .z);
}



void
QtUtil :: Grid3DView :: window_to_ik (int x, int y, int * i, int * k)
const
{
	const int SIZE = 2 * m_width + 1;

	if (i)
		*i = (SIZE * x) / width () - m_width;

	if (k)
		*k = (SIZE * ((height () - 1) - y)) / height () - m_width;
}


void
QtUtil :: Grid3DView :: mousePressEvent (QMouseEvent * e)
{
	int i, k;

	window_to_ik (e -> pos () .x (), e -> pos () .y (), & i, & k);

	const auto at = m_position + to_global (i, k);

	set_cursor (at .x, at .y, at .z);
}



QtUtil :: Grid3DView :: Position
QtUtil :: Grid3DView :: to_global (int i, int k)
const
{
	switch (m_plane)
	{

	case Plane :: XY:
		return {i, k, 0};

	case Plane :: XZ:
		return {i, 0, k};

	case Plane :: YZ:
		return {0, k, i};

	default:
		throw std :: logic_error (__PRETTY_FUNCTION__);
	};
}



void
QtUtil :: Grid3DView :: to_cursor ()
{
	set_position (m_cursor .x, m_cursor .y, m_cursor .z);
}



void
QtUtil :: Grid3DView :: to_cursor_plane ()
{
	switch (m_plane)
	{

	case Plane :: XY:
		set_position (m_position .x, m_position .y, m_cursor .z);
		break;

	case Plane :: XZ:
		set_position (m_position .x, m_cursor .y, m_position .z);
		break;

	case Plane :: YZ:
		set_position (m_cursor .x, m_position .y, m_position .z);
		break;
	};
}



void
QtUtil :: Grid3DView :: set_cursor (int x, int y, int z)
{
	if (m_cursor != Position (x, y, z))
	{
		m_cursor = {x, y, z};

		if (0 == z)
		{
			++z;
			--z;
			++z;
			--z;
		}

		emit cursor_changed (x, y, z);

		update ();
	}
}



QtUtil :: Grid3DView :: SharedState *
QtUtil :: Grid3DView :: create_shared_state ()
const
{
	return new SharedState ();
}



QtUtil :: Grid3DView :: SharedState &
QtUtil :: Grid3DView :: shared_state ()
{
	if (! m_shared_state)
	{
		m_shared_state .reset (create_shared_state ());

		m_shared_state -> m_views .insert (this);
	}

	return * m_shared_state;
}



const QtUtil :: Grid3DView :: SharedState &
QtUtil :: Grid3DView :: shared_state ()
const
{
	if (! m_shared_state)
	{
		auto self = const_cast <Grid3DView *> (this);
		
		self -> m_shared_state .reset (create_shared_state ());
		self -> m_shared_state -> m_views .insert (self);
	}

	return * m_shared_state;
}



void
QtUtil :: Grid3DView :: join (Grid3DView * other)
{
	assert (other);

	if (! m_shared_state && other -> m_shared_state)
	{
		other -> join (this);
		return;
	}

	if (! m_shared_state)
		// force create
		shared_state ();

	if (m_shared_state == other -> m_shared_state)
	{
		assert (! m_shared_state
		     || m_shared_state -> m_views .contains (other));

		assert (! other -> m_shared_state
		     || other -> m_shared_state -> m_views .contains (this));

		return;
	}

	// I have state. Other may not.

	assert (m_shared_state);

	assert (  m_shared_state -> m_views .contains (this));
	assert (! m_shared_state -> m_views .contains (other));

	if (! other -> m_shared_state)
	{
		// Copy state and simple sync.

		other -> m_shared_state = m_shared_state;

		for (auto i : m_shared_state -> m_views)
			sync (i, other); // includes this

		m_shared_state -> m_views .insert (other);
	}
	else
	{
		auto views = m_shared_state -> m_views;
		auto other_views = other -> m_shared_state -> m_views;
		auto other_state = other -> m_shared_state;

		assert (other_views .contains (other));
		assert (! other_views .contains (this));

		assert (typeid (m_shared_state .get ())
		     == typeid (other -> m_shared_state .get ()));

		assert (0 ==
		QSet <Grid3DView *> (other_views) .intersect (views) .size ());

		// Pairwise sync. Add each other to this views. All others
		// get this shared state.

		for (auto i : views)
			for (auto k : other_views)
				sync (i, k);

		for (auto i : other_views)
		{
			assert (i -> m_shared_state == other -> m_shared_state);

			i -> m_shared_state = m_shared_state;
			
			m_shared_state -> m_views .insert (other);
		}
	}

	assert (m_shared_state == other -> m_shared_state);
	assert (m_shared_state -> m_views .contains (this));
	assert (m_shared_state -> m_views .contains (other));
}



void
QtUtil :: Grid3DView :: sync
(
	QtUtil :: Grid3DView * a,
	QtUtil :: Grid3DView * b
)
{
	QObject :: connect (
		a, SIGNAL (cursor_changed (int, int, int)),
		b, SLOT   (set_cursor     (int, int, int)));

	QObject :: connect (
		b, SIGNAL (cursor_changed (int, int, int)),
		a, SLOT   (set_cursor     (int, int, int)));
}



void
QtUtil :: Grid3DView :: update_joined ()
{
	each_joined ([] (Grid3DView * v) {v -> update ();});
}



const QtUtil :: Grid3DView :: Position &
QtUtil :: Grid3DView :: position ()
const
{
	return m_position;
}



const QtUtil :: Grid3DView :: Position &
QtUtil :: Grid3DView :: cursor ()
const
{
	return m_cursor;
}



QtUtil :: Grid3DView :: Plane
QtUtil :: Grid3DView :: plane ()
const
{
	return m_plane;
}
